#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include<time.h>
#include "GameUtil.h"

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 50      

//#define MESSAGE_SIZE 22
#define PART_SIZE 10
#define MESSAGE_SIZE 22
#define USER_FORMATING 22
#define QUESTION_SIZE 510
#define OPTION_SIZE 100

// declaration globale du mutex et tableau global accessible par tous les threads
HANDLE mutex;
char** GlobalQuesMatrix;
// variable pour verifier si la matrix de question a deja ete chargée
int MatrixLoaded=0;
int *PtrMatrixLoaded = &MatrixLoaded;

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

/*
 void HandleLogin(int *ResponseFlag, char *MessageBuffer) {
    FILE *myfile = fopen("Users.txt", "r");
    if (myfile == NULL) {
        printf("Erreur d'ouverture du fichier.\n");
        *ResponseFlag = -1;  // Erreur d'ouverture du fichier
        return;
    }

    char enteredName[PART_SIZE] = {0};
    char enteredPassword[PART_SIZE] = {0};

    // Extraction du nom d'utilisateur
    int i = 0;
    while (MessageBuffer[i] != '*' && i < PART_SIZE) {
        enteredName[i] = MessageBuffer[i];
        i++;
    }
    enteredName[i] = '\0';  // Fin de chaîne

    // Extraction du mot de passe
    i = 0;
    while (MessageBuffer[PART_SIZE + i] != '*' && i < PART_SIZE) {
        enteredPassword[i] = MessageBuffer[PART_SIZE + i];
        i++;
    }
    enteredPassword[i] = '\0';  // Fin de chaîne

    char fileLine[MESSAGE_SIZE];
    int found = 0;

    // Lecture du fichier ligne par ligne
    while (fgets(fileLine, sizeof(fileLine), myfile)) {
        fileLine[strcspn(fileLine, "\n")] = 0;  // Enlever le '\n' à la fin

        char fileName[PART_SIZE] = {0};
        char filePassword[PART_SIZE] = {0};

        // Extraction des informations depuis la ligne du fichier
        if (sscanf(fileLine, "Nom: %s, Mot de passe: %s", fileName, filePassword) == 2) {
            // Comparaison avec les données entrées
            if (strcmp(enteredName, fileName) == 0 && strcmp(enteredPassword, filePassword) == 0) {
                found = 1;
                break;
            }
        }
    }

    fclose(myfile);

    // Vérification de la connexion
    if (found) {
        printf("Connexion réussie pour %s.\n", enteredName);
        *ResponseFlag = 3;  // Connexion réussie
    } else {
        printf("Nom d'utilisateur ou mot de passe incorrect.\n");
        *ResponseFlag = -1;  // Échec de la connexion
    }
}
*/
void HandleLogin(int *ResponseFlag, char *MessageBuffer) {
    FILE *myfile = fopen("Users.txt", "r");
    if (myfile == NULL) {
        printf("Erreur d'ouverture du fichier.\n");
        *ResponseFlag = -1;  // Échec d'ouverture du fichier
        return;
    }

    char enteredName[PART_SIZE] = {0};
    char enteredPassword[PART_SIZE] = {0};

    // Extraction du nom d'utilisateur depuis MessageBuffer
    int i = 0;
    while (MessageBuffer[i] != '*' && i < PART_SIZE) {
        enteredName[i] = MessageBuffer[i];
        i++;
    }
   // enteredName[i] = '\0';  // Fin de chaîne

    // Extraction du mot de passe depuis MessageBuffer
    i = 0;
    while (MessageBuffer[PART_SIZE + i] != '*' && i < PART_SIZE) {
        enteredPassword[i] = MessageBuffer[PART_SIZE + i];
        i++;
    }
   // enteredPassword[i] = '\0';  // Fin de chaîne

    printf("Nom d'utilisateur entré : %s\n", enteredName);
   // printf("Mot de passe entré : %s\n", enteredPassword);

    char fileLine[MESSAGE_SIZE];
    int found = 0;

    // Lecture du fichier ligne par ligne
    
    while (fgets(fileLine, sizeof(fileLine), myfile)) {
      //  fileLine[strcspn(fileLine, "\n")] = 0;  // Enlever le '\n' à la fin

        char fileName[PART_SIZE] = {0};
        char filePassword[PART_SIZE] = {0};

        // Extraction des informations depuis la ligne du fichier
        printf(" dedans\n");
        if (sscanf(fileLine, "Nom:%s Pwd:%s\n", fileName, filePassword) == 2) {
            // Comparaison avec les données entrées

            printf( " nom d'utilisateur dans le file : %s  \n", fileName);
            if (strcmp(enteredName, fileName) == 0 && strcmp(enteredPassword, filePassword) == 0) {
                printf("Connexion réussie pour %s.\n", enteredName);
                *ResponseFlag = 3;  // Connexion réussie
                fclose(myfile);  // Fermeture du fichier immédiatement après succès
                return;  // Quitter immédiatement dès qu'un utilisateur est trouvé
            }
        }
    }

    fclose(myfile);

    // Si aucun utilisateur valide trouvé, envoyer un message d'erreur
    printf("Nom d'utilisateur ou mot de passe incorrect.\n");
    *ResponseFlag = -1;  // Échec de la connexion
}
/*
void HandleSignUp(int *ResponseFlag, char *MessageBuffer) {
    FILE *myfile = fopen("Users.txt", "a+");
    if (myfile == NULL) {
        printf("Erreur d'ouverture du fichier.\n");
        return;
    }

    char username[PART_SIZE] = {0};
    char password[PART_SIZE] = {0};

    int i = 0;
    while (MessageBuffer[i] != '*' && i < PART_SIZE) {
        username[i] = MessageBuffer[i];
        i++;
    }
    username[i] = '\0';  // Ajouter le caractère de fin de chaîne

    i = 0;
    while (MessageBuffer[PART_SIZE + i] != '*' && i < PART_SIZE) {
        password[i] = MessageBuffer[PART_SIZE + i];
        i++;
    }
    password[i] = '\0';  // Ajouter le caractère de fin de chaîne

    // Sauvegarde dans la base de données
    fprintf(myfile, "Nom: %s, Mot de passe: %s\n", username, password);

    fclose(myfile);

    *ResponseFlag = 4;
}
*/
void HandleSignUp(int *ResponseFlag, char *MessageBuffer) {
    FILE *myfile = fopen("Users.txt", "a+");
    if (myfile == NULL) {
        printf("Erreur d'ouverture du fichier.\n");
        *ResponseFlag = -1;
        return;
    }

    char username[PART_SIZE] = {0};
    char password[PART_SIZE] = {0};

    // Extraction du nom d'utilisateur
    int i = 0;
    while (MessageBuffer[i] != '*' && i < PART_SIZE) {
        username[i] = MessageBuffer[i];
        i++;
    }
    //username[i] = '\0';  // Fin de chaîne

    // Extraction du mot de passe
    i = 0;
    while (MessageBuffer[PART_SIZE + i] != '*' && i < PART_SIZE) {
        password[i] = MessageBuffer[PART_SIZE + i];
        i++;
    }
    //password[i] = '\0';  // Fin de chaîne

    // Sauvegarde dans le fichier

    //printf(" pwd avant de sauvegarder , %s \n", password);
    fprintf(myfile, "Nom:%s Pwd:%s\n", username, password);

    fclose(myfile);

    *ResponseFlag = 4;  // Indication d'un succès d'inscription
}

void HandleGameLunch( ){
     printf(" I am doing GameLunch \n");      
}
 void HandlePlayGame(char * MessageBuffer, SOCKET socketFd) {
    // extraction du numéro de questions dans le message buffer
    char StrQuestionNumber[5];
    int i;
    for(i = 0; MessageBuffer[i] != '*'; i++) {
        StrQuestionNumber[i] = MessageBuffer[i];
    }

    printf("SUR LE POINT DE JOUER A UN JEU\n");  
    int QuestionNumber = atoi(StrQuestionNumber);
    printf("le nombre de questions reçu est : %d\n", QuestionNumber);
    int IamLoader = 0;

    // fonction pour permettre à seulement un seul player de créer un jeu grâce à la variable IamLoaded
    if (*PtrMatrixLoaded == 0) {
        IamLoader = 1;
        WaitForSingleObject(mutex, INFINITE);
        GlobalQuesMatrix = Prepared_Ques_Arr(QuestionNumber);
        Prepare_Quiz(GlobalQuesMatrix, &QuestionNumber);
        ReleaseMutex(mutex);
    }

    // message de confirmation que le jeu a été bien chargé
    MessageBuffer[0] = '1';
    int ConfirmGameCreationFd = send(socketFd, MessageBuffer, MESSAGE_SIZE, 0);
    if (ConfirmGameCreationFd == SOCKET_ERROR) {
        printf("ERREUR LORS DE LA CONFIRMATION DE CREATION DU JEU : %d\n", WSAGetLastError());
        return;
    }

    // Envoi des questions et réception des réponses
    char Question[OPTION_SIZE] = {0};
    char Option1[OPTION_SIZE] = {0};
    char Option2[OPTION_SIZE] = {0};
    char Option3[OPTION_SIZE] = {0};
    char Option4[OPTION_SIZE] = {0};
    char GoodOpt[OPTION_SIZE] = {0};
    char UserOpt[OPTION_SIZE]={0};
    char QuestionMessageBuffer[QUESTION_SIZE] = {0};

    for(i = 0; i < QuestionNumber; i++) {
        int SendQuestionFlag, RecvQuestionFd;
        sscanf(GlobalQuesMatrix[i], "ID:%*d QUESTION:%99[^#] # OPTION1:%99[^#] # OPTION2:%99[^#] # OPTION3:%99[^#] # OPTION4:%99[^#] # CORRECTOPTION:%99[^#] #", 
               Question, Option1, Option2, Option3, Option4, GoodOpt);

        // Charger les différentes sections dans notre buffer
        strncpy(QuestionMessageBuffer, Question, OPTION_SIZE);
        strncpy(QuestionMessageBuffer + OPTION_SIZE, Option1, OPTION_SIZE);
        strncpy(QuestionMessageBuffer + 2 * OPTION_SIZE, Option2, OPTION_SIZE);
        strncpy(QuestionMessageBuffer + 3 * OPTION_SIZE, Option3, OPTION_SIZE);
        strncpy(QuestionMessageBuffer + 4 * OPTION_SIZE, Option4, OPTION_SIZE);
        //strncpy(QuestionMessageBuffer + 5 * OPTION_SIZE, GoodOpt, OPTION_SIZE);

        // Ajouter un flag pour indiquer qu'il s'agit d'une question (flag 9)
        QuestionMessageBuffer[QUESTION_SIZE - 1] = '9';

        // Envoi de la question au client
        SendQuestionFlag = send(socketFd, QuestionMessageBuffer, QUESTION_SIZE, 0);
        if(SendQuestionFlag == SOCKET_ERROR) {
            printf("ERREUR LORS DE L'ENVOI DE QUESTION numéro %d, erreur : %s", i, WSAGetLastError());
        }

        memset(QuestionMessageBuffer, 0, QUESTION_SIZE);

        // Attendre la réponse du client avant d'envoyer la question suivante
        printf("QUESTION NUMÉRO %d... EN ATTENTE DE RÉPONSE\n", i);

        RecvQuestionFd = recv(socketFd, QuestionMessageBuffer, QUESTION_SIZE, 0);
        if(RecvQuestionFd == SOCKET_ERROR) {
            printf("ERREUR LORS DE LA RÉPONSE DU CLIENT À LA QUESTION numéro %d, erreur : %s", i, WSAGetLastError());
        }
        // gymnastique pour pouvoir verifier la veracité de notre reponse
       strncpy(UserOpt, QuestionMessageBuffer, OPTION_SIZE);
       int CmpResult = strncmp( GoodOpt, UserOpt, strlen(GoodOpt) );
       if(CmpResult == 0)
       {
        printf(" BONNE REPONSE \n");
       }else
       {
        printf(" MAUVAISE REPONSE \n");
       }

        memset(QuestionMessageBuffer, 0, QUESTION_SIZE);  // Réinitialisation du buffer
    }

    printf("Le jeu est en train d'être joué\n");

    // Libérer la mémoire si le serveur est le créateur du jeu
    if (IamLoader == 1) {
        IamLoader = 0;
        for (int i = 0; i < QuestionNumber; i++) {

           /* if (strlen(GlobalQuesMatrix[i]) > 0) {
                printf(" %s\n", GlobalQuesMatrix[i]);
            }*/
            free(GlobalQuesMatrix[i]);
        }
        free(GlobalQuesMatrix);
    }
    printf("Mémoire libérée proprement\n");
}
 


int getFlag(char *buffer) {
 
 char charFlag; 
    int response;
    
    charFlag = *(buffer + MESSAGE_SIZE - 1);   
    
    // Convertir charFlag en une chaîne pour utiliser atoi()
    char strFlag[2];   
    strFlag[0] = charFlag;  
    strFlag[1] = '\0';   
    
    
    response = atoi(strFlag);
    
    printf("\nLa valeur retournée par getFlag est : %d\n", response);
    
    return response;
}

void DoSomeAction(int flag, int* Responseflag, char * MessageBuffer , SOCKET socket ){
    printf(" bien dans do some action \n");
    switch (flag)
    {
    case 1:
         HandleGameLunch();
        break;
    case 2:
       printf(" option 2");
        break;
    case 3 :       
         HandleLogin(Responseflag, MessageBuffer);
        break;
    case 4:
        HandleSignUp(Responseflag, MessageBuffer);
        break;    
     case 8:
        HandlePlayGame(MessageBuffer, socket);
        break; 
    default:
    printf(" do some action");
        break;
    }

}

//  client handler
DWORD WINAPI ClientHandler(LPVOID clientSocket) {
    int ResponseFlag;
    SOCKET clientSock = *((SOCKET*)clientSocket);
    char buffer[MESSAGE_SIZE] = {0};

    // Boucle de réception continue pour ce client
  
    while (1) {
       //  on recoit le message du client;
       ResponseFlag = 0;
        int recvResult = recv(clientSock, buffer, MESSAGE_SIZE, 0);
        if (recvResult > 0) {
             
            printf("Message du client : %s \n", buffer);

            // Vérifier si le client envoie un message de déconnexion
            if (strncmp(buffer, "exit", 4) == 0) {
                printf("Client déconnecté.\n");
                break;
            }
            
            // on get le flag pour pouvoir bien interpreter le message du client
            // on cherche le flag de la requette
           int flag = getFlag(buffer);
           DoSomeAction(flag, &ResponseFlag, buffer, clientSock);
 
            //  apres avoir fait une action conformement on flage je renvoi un message au client pour lui notifier de la reponse 

            memset(buffer, '*', MESSAGE_SIZE);
             
            buffer[MESSAGE_SIZE-1]= ResponseFlag+'0';
            printf(" le flag de reponse a ete %d \n", ResponseFlag);

             int SentMessageFd = send(clientSock, buffer, MESSAGE_SIZE, 0);
           if (SentMessageFd == SOCKET_ERROR) {
                 printf("Erreur d'envoi du message: %d\n", WSAGetLastError());
               }


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

  typedef struct {
    int UserId;
    char nom[PART_SIZE];
    char password[PART_SIZE];
} User;

typedef enum{
    messageError =1,
    messageSuccess=2, 
    LoginF=3,
    NewAccount=4,
    GiveAnswer=5,
    LoginTrue=6,
    LoginFalse=7,
} MessageProtocol;


int getLastIdId()
{
FILE * IdFile = fopen("Id.txt", "r");
char IdChar ;
int Id;
if(IdFile!=NULL){
     IdChar = fgetc(IdFile);
      Id =  IdChar - '0';
fclose(IdFile);
return Id;
}
printf("-------UNE ERREUR EST SURVENUE LORS DE L'ACCES A L'ID -------");

return -1;
}

void HandleSingUp2( User user )
{ // get Id
char buffer[USER_FORMATING]={0};
int Id = getLastIdId();
if(Id != -1){
    // incrementer l'Id
Id++;
strcpy(buffer, user.nom);
strcpy( buffer+(USER_FORMATING/2 )-1 ,user.password );
  sprintf(buffer + USER_FORMATING - 1, "%d", Id);
FILE * UserFile = fopen("IdNameAndPassword.txt", "a+");
if(UserFile != NULL){


fwrite(buffer, USER_FORMATING, 1, UserFile);
fclose(UserFile);
printf("------------NOUVEAU UTILISATEUR CREE , VEILLEZ VOUS CONNECTER------------");
}
printf("------------ERREUR SURVENUE LORS DE L'OUVERTURE  DU FICHIER IDNAMEPSW------------");

}


}

int main() {
    InitializeWinsock();
     // initialisation de mon mutex
     mutex = CreateMutex( NULL, FALSE, NULL);
     // j'initialise mon timer
       srand(time(NULL));
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
