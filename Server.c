#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 50 // taill max de client

// init winsocl
void InitializeWinsock() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("Erreur d'initialisation de Winsock: %d\n", WSAGetLastError());
        exit(1);
    }
}

//  netoyer winsock
void CleanupWinsock() {
    WSACleanup();
}

//  client handler
DWORD WINAPI ClientHandler(LPVOID clientSocket) {
    SOCKET clientSock = *((SOCKET*)clientSocket);
    char buffer[BUFFER_SIZE] = {0};

    // Boucle de réception continue pour ce client
    while (1) {
        int recvResult = recv(clientSock, buffer, BUFFER_SIZE, 0);
        if (recvResult > 0) {
            printf("Message du client : %s\n", buffer);

            // Vérifier si le client envoie un message de déconnexion
            if (strncmp(buffer, "exit", 4) == 0) {
                printf("Client déconnecté.\n");
                break;
            }

            // Réinitialiser mon buffer
            memset(buffer, 0, BUFFER_SIZE);
        } else if (recvResult == 0) {
            printf("Client déconnecté proprement.\n");
            break;
        } else {
            printf("Erreur de réception du message : %d\n", WSAGetLastError());
            break;
        }
    }

    // je ferme le socket
    closesocket(clientSock);
    return 0;
}

 
int main() {
    InitializeWinsock();

    //  
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Erreur de création du socket: %d\n", WSAGetLastError());
        CleanupWinsock();
        exit(1);
    }

    // socket set
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5600);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Binding 
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Erreur de liaison du socket: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        CleanupWinsock();
        exit(1);
    }

    // ecoute
    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Erreur d'écoute sur le socket: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        CleanupWinsock();
        exit(1);
    }

    printf("Serveur en écoute, en attente de connexions...\n");


// ma boucle d'acceptation infinie
    while (1) {
        struct sockaddr_in clientAddress;
        int clientAddressLen = sizeof(clientAddress);

        // Accepter une nouvelle connexion client
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Erreur d'acceptation du client: %d\n", WSAGetLastError());
            continue;
        }

        printf("Nouveau client connecté.\n");

        // Création d'un thread pour gérer le client
        HANDLE hThread;
        DWORD threadId;
        hThread = CreateThread(
            NULL,               // attributs de sécurité
            0,                  // taille de la pile
            ClientHandler,      // fonction de thread
            &clientSocket,      // argument pour le thread
            0,                  // options de création
            &threadId           // identifiant du thread
        );

        if (hThread == NULL) {
            printf("Erreur de création du thread pour le client: %d\n", GetLastError());
            closesocket(clientSocket);
        } else {
            // Fermeture du handle du thread
            CloseHandle(hThread);
        }
    }

    // Fermeture du socket serveur
    closesocket(serverSocket);
    CleanupWinsock();

    return 0;
}
