#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include<string.h>

#define BUFFER_SIZE 1024

int main() {
    //  initialisation de winsock
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("Erreur d'initialisation: %d\n", WSAGetLastError());
        exit(1);
    }

    //  je cree le socket
    SOCKET SocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFd == INVALID_SOCKET) {
        printf("Erreur de création du socket: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    // configuration
    struct sockaddr_in SocketAddress;
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(5600);

    int inetReturnCode = inet_pton(AF_INET, "192.168.1.124", &SocketAddress.sin_addr);
    if (inetReturnCode <= 0) {
        printf("Erreur d'adresse IP.\n");
        closesocket(SocketFd);
        WSACleanup();
        exit(1);
    }

    printf("Connexion au serveur...\n");

    //  connection
    if (connect(SocketFd, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress)) == SOCKET_ERROR) {
        printf("Erreur de connexion: %d\n", WSAGetLastError());
        closesocket(SocketFd);
        WSACleanup();
        exit(1);
    }

    printf("Connexion acceptée.\n");

    char MessageBuffer[BUFFER_SIZE] = {0};

    //  boucle de message
    while (1) {
        printf("Entrez le message à envoyer : ");
        fgets(MessageBuffer, sizeof(MessageBuffer), stdin);

        int SentMessageFd = send(SocketFd, MessageBuffer, strlen(MessageBuffer), 0);
        if (SentMessageFd == SOCKET_ERROR) {
            printf("Erreur d'envoi du message: %d\n", WSAGetLastError());
            break;
        }

            if( ! strncmp(MessageBuffer, "exit", 4)  ){
                break;
            }
        //  buffer renew
        memset(MessageBuffer, 0, BUFFER_SIZE);
    }

    // Fermeture du socket
    closesocket(SocketFd);
    WSACleanup();

    return 0;
}
