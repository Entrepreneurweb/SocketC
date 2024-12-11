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

// message protocol enums
#define MESSAGE_ERROR   "1"
#define MESSAGE_SUCCES  "2"
#define LOGIN           "3"
#define NEW_ACCOUNT     "4"
#define ANSWER_QUESTION "5"
#define CREATE_SESSION  "11"
#define JOIN_SESSION    "12"
#define GET_SESSION_LIST "13"

// declaration globale du mutex et tableau global accessible par tous les threads
HANDLE mutex;
char** GlobalQuesMatrix;


// *************Party configurations
 

// variable pour verifier si la matrix de question a deja ete chargée
int MatrixLoaded=0;
int *PtrMatrixLoaded = &MatrixLoaded;
int Global_Question_Number=0;
int *PtrGlobal_Question_Number = &Global_Question_Number;


void Handle_Check_MatrixLoaded(SOCKET socket , char * MessageBuffer){
   printf(" in Handel_Check_MatrixLoaded  test  \n");
    if( *PtrMatrixLoaded == 1)
    {
        MessageBuffer[1]='1';
         printf(" in Handel_Check_MatrixLoaded  1  \n");
         printf(" MESSAGE BUFFER DE MATRIX LOADED %s \n", MessageBuffer);
        send(socket, MessageBuffer, MESSAGE_SIZE, 0);
    }else if( *PtrMatrixLoaded == 0 ) {
        MessageBuffer[1]='0'; 
         printf(" in Handel_Check_MatrixLoaded  0 \n");
          printf(" MESSAGE BUFFER DE MATRIX LOADED %s \n", MessageBuffer);
        send(socket, MessageBuffer, MESSAGE_SIZE, 0);
    }else{
        MessageBuffer[1]='7'; 
         printf(" MESSAGE BUFFER DE MATRIX LOADED %s \n", MessageBuffer);
         printf(" in Handel_Check_MatrixLoaded  0 \n");
        send(socket, MessageBuffer, MESSAGE_SIZE, 0);
    }
}

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

int Get_Last_Id()
{
    FILE *IdFile = fopen("Id.txt", "r");
    int Id;
    
    if (IdFile != NULL) {
        // Lire un entier à partir du fichier
        if (fscanf(IdFile, "%d", &Id) == 1) {
            fclose(IdFile);
            return Id;
        } else {
            printf("-------ERREUR : Format incorrect dans le fichier Id.txt-------\n");
        }
        fclose(IdFile);
    } else {
        printf("-------UNE ERREUR EST SURVENUE LORS DE L'ACCES A L'ID -------\n");
    }
    
    return -1;
}
int Update_Last_Id(int newId)
{
    FILE *IdFile = fopen("Id.txt", "w");  // Ouvrir le fichier en mode écriture
    if (IdFile != NULL) {
        // Écrire le nouvel ID dans le fichier
        fprintf(IdFile, "%d", newId);
        fclose(IdFile);
        return 1;  // Succès
    } else {
        printf("-------ERREUR : Impossible d'ouvrir le fichier pour mise à jour de l'ID-------\n");
        return 0;  // Échec
    }
}
void Handle_Login(int *ResponseFlag, char *MessageBuffer, int * ptr_User_Id , int * Ptr_User_Score) {
    FILE *myfile = fopen("Users.txt", "r");
    if (myfile == NULL) {
        printf("Erreur d'ouverture du fichier.\n");
        *ResponseFlag = -1;  // Échec d'ouverture du fichier
        return;
    }

    char enteredName[PART_SIZE] = {0};
    char enteredPassword[PART_SIZE] = {0};
printf(" message buffer dans handle login : %s", MessageBuffer);
    // Extraction du nom d'utilisateur depuis MessageBuffer
    int i = 0;
    while (MessageBuffer[i] != '*' && i < PART_SIZE) {
        enteredName[i] = MessageBuffer[i];
        i++;
    }
   // enteredName[i] = '\0';  // Fin de chaîne

    // Extraction du mot de passe depuis MessageBuffer
    i = 0;
    while (MessageBuffer[PART_SIZE +i-1] != '*' && i < PART_SIZE) {
        enteredPassword[i] = MessageBuffer[PART_SIZE + i-1];
        i++;
    }
   // enteredPassword[i] = '\0';  // Fin de chaîne

    printf("Nom d'utilisateur entré : %s\n", enteredName);
    printf("Mot de passe entré : %s\n", enteredPassword);

    char fileLine[256];
    int found = 0;

    // Lecture du fichier ligne par ligne
    
   while (fgets(fileLine, sizeof(fileLine), myfile)) {
    //fileLine[strcspn(fileLine, "\n")] = 0;  // Supprimer le '\n' à la fin

    char fileName[PART_SIZE] = {0};
    char filePassword[PART_SIZE] = {0};

    // Extraction des informations depuis la ligne du fichier
    printf("dedans\n");
    int Id;
    int Score;
    if (sscanf(fileLine, "Id:%d Nom:%s Pwd:%s TotalScore:%d", &Id, fileName, filePassword,&Score) == 4) {
        // Comparaison avec les données entrées
        printf("Nom d'utilisateur dans le fichier : %s\n", fileName);
        printf("mot de passe  d'utilisateur dans le fichier : %s\n", filePassword);
        if (strcmp(enteredName, fileName) == 0 && strcmp(enteredPassword, filePassword) == 0) {
            printf("Connexion réussie pour %s.\n", enteredName);
            *ptr_User_Id = Id;
            *Ptr_User_Score=Score;
            printf(" Login Succes User Id:%d , Score:  %d  \n", *ptr_User_Id, *Ptr_User_Score);
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
 
void Handle_SignUp(int *ResponseFlag, char *MessageBuffer) {
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
    int Id = Get_Last_Id();
    int TotalScore=0;
    fprintf(myfile, "Id:%d Nom:%s Pwd:%s TotalScore:%d\n",Id, username, password, TotalScore);

    Id++;

    Update_Last_Id(Id);

    fclose(myfile);

    *ResponseFlag = 4;  // Indication d'un succès d'inscription
}

void Handle_GameLunch( ){
     printf(" I am doing GameLunch \n");      
}

double CalculatePlayerScore(int UserScore, int QuestionNumber) {
    // Vérification pour éviter une division par zéro
    if (QuestionNumber <= 0) {
        printf("Erreur : le nombre de questions doit être supérieur à 0.\n");
        return -1.0; // Retourne une valeur d'erreur spéciale
    }

    // Calcul de la note sur 100
    double ScorePercentage = ((double)UserScore / QuestionNumber) * 100;

    // Retourner la note sur 100
    return ScorePercentage;
}

 void UpdateUserScore(int userId, int scoreToAdd, int* Ptr_Rank, int* Ptr_Total_Player ) {
    FILE *file = fopen("Users.txt", "r+");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier Users.txt.\n");
        return;
    }

    char buffer[256];
    char lines[100][256]; // Tableau pour stocker jusqu'à 100 lignes
    int scores[100];      // Tableau des scores pour le tri
    int ids[100];         // Tableau des IDs
    char names[100][50];  // Tableau des noms
    char passwords[100][50]; // Tableau des mots de passe
    int userCount = 0;
    int found = 0;

    // Lecture et extraction des données
    while (fgets(buffer, sizeof(buffer), file)) {
        int id, totalScore;
        char name[50], password[50];

        if (sscanf(buffer, "Id:%d Nom:%49s Pwd:%49s TotalScore:%d", &id, name, password, &totalScore) == 4) {
            // Stocker les informations
            ids[userCount] = id;
            scores[userCount] = totalScore;
            strcpy(names[userCount], name);
            strcpy(passwords[userCount], password);
            strcpy(lines[userCount], buffer);

            // Mise à jour du score si l'utilisateur est trouvé
            if (id == userId) {
                scores[userCount] += scoreToAdd;
                snprintf(lines[userCount], sizeof(lines[userCount]),
                         "Id:%d Nom:%s Pwd:%s TotalScore:%d\n", id, name, password, scores[userCount]);
                found = 1;
            }
            userCount++;
        }
    }

    if (!found) {
        printf("Utilisateur avec Id:%d non trouvé dans Users.txt.\n", userId);
        fclose(file);
        return;
    }

    // Tri des utilisateurs par scores décroissants
    for (int i = 0; i < userCount - 1; i++) {
        for (int j = i + 1; j < userCount; j++) {
            if (scores[i] < scores[j]) {
                // Échange des scores
                int tempScore = scores[i];
                scores[i] = scores[j];
                scores[j] = tempScore;

                // Échange des IDs
                int tempId = ids[i];
                ids[i] = ids[j];
                ids[j] = tempId;

                // Échange des noms
                char tempName[50];
                strcpy(tempName, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], tempName);

                // Échange des mots de passe
                char tempPassword[50];
                strcpy(tempPassword, passwords[i]);
                strcpy(passwords[i], passwords[j]);
                strcpy(passwords[j], tempPassword);

                // Échange des lignes
                char tempLine[256];
                strcpy(tempLine, lines[i]);
                strcpy(lines[i], lines[j]);
                strcpy(lines[j], tempLine);
            }
        }
    }

    // Trouver le rang du joueur
    int rank = 1;
    for (int i = 0; i < userCount; i++) {
        if (ids[i] == userId) {
            printf("You are ranked #%d with a total score of %d.\n", rank, scores[i]);
            
            *Ptr_Rank=rank;
            *Ptr_Total_Player = scores[i];
            break;
        }
        rank++;
    }

    // Réécriture du fichier
    freopen("Users.txt", "w", file);
    for (int i = 0; i < userCount; i++) {
        fputs(lines[i], file);
    }

    fclose(file);
}

void IntToString(int number, char *output) {
    // Utilisation de sprintf pour convertir un entier en chaîne de caractères
    sprintf(output, "%d", number);
}

 void HandlePlayGame(char * MessageBuffer, SOCKET socketFd,  int * Ptr_User_Id, int * Ptr_User_Score) {
int UserScore=0;
 int QuestionNumber;
char ** Local_Question_Matrix;
 int IamLoader = 0;
 

  // extraction du numéro de questions dans le message buffer
    char StrQuestionNumber[5];
    int i;
    for(i = 0; MessageBuffer[i] != '*'; i++) {
        StrQuestionNumber[i] = MessageBuffer[i];
    }

    printf("SUR LE POINT DE JOUER A UN JEU\n");  
     QuestionNumber = atoi(StrQuestionNumber);
    printf("le nombre de questions reçu est : %d\n",  QuestionNumber );
   

    // fonction pour permettre à seulement un seul player de créer un jeu grâce à la variable IamLoaded
    
        *PtrMatrixLoaded =1;
        IamLoader = 1;
      //  WaitForSingleObject(mutex, INFINITE);
        Local_Question_Matrix = Prepared_Ques_Arr( QuestionNumber);
        
        Prepare_Quiz(Local_Question_Matrix, &QuestionNumber);
      
       // ReleaseMutex(mutex);
    

    // message de confirmation que le jeu a été bien chargé
   // memset(MessageBuffer, '1', MESSAGE_SIZE);
     MessageBuffer[0] = '1';
    int ConfirmGameCreationFd = send(socketFd, MessageBuffer, MESSAGE_SIZE, 0);
    printf("MessageBuffer : %s \n", MessageBuffer);
    if (ConfirmGameCreationFd == SOCKET_ERROR) {
        printf("ERREUR LORS DE LA CONFIRMATION DE CREATION DU JEU : %d\n", WSAGetLastError());
        return;
    }



    printf("le nombre de questions reçu est  toujours: %d\n", QuestionNumber);
  
    // Envoi des questions et réception des réponses
    char Question[OPTION_SIZE] = {0};
    char Option1[OPTION_SIZE] = {0};
    char Option2[OPTION_SIZE] = {0};
    char Option3[OPTION_SIZE] = {0};
    char Option4[OPTION_SIZE] = {0};
    char GoodOpt[OPTION_SIZE] = {0};
    char UserOpt[OPTION_SIZE]={0};
    char QuestionMessageBuffer[QUESTION_SIZE] = {0};

    for(int i = 0; i < QuestionNumber; i++) {
        int SendQuestionFlag, RecvQuestionFd;
        sscanf(Local_Question_Matrix[i], "ID:%*d QUESTION:%99[^#] # OPTION1:%99[^#] # OPTION2:%99[^#] # OPTION3:%99[^#] # OPTION4:%99[^#] # CORRECTOPTION:%99[^#] #", 
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
        UserScore++;
        
       }else
       {
        printf(" MAUVAISE REPONSE \n");
       }

        memset(QuestionMessageBuffer, 0, QUESTION_SIZE);  // Réinitialisation du buffer
    }
    
    double Player_Note = CalculatePlayerScore( UserScore, QuestionNumber );
    printf(" player got %lf \n",Player_Note );
    char buffer[4];
    int rank=0;
    int * Ptr_Rank=&rank;
    int TotalPlayers=0;
    int * Ptr_Total_Player = &TotalPlayers;
    UpdateUserScore(*Ptr_User_Id, UserScore ,Ptr_Rank, Ptr_Total_Player );
    printf(" Ptr val : rank  %d  Score %d \n", rank, TotalPlayers);
    snprintf(buffer, sizeof(buffer), "%.1f", Player_Note); 
    strncpy(MessageBuffer, buffer, strlen(buffer));

    char RankStr[5];
    char TotalScore[5];
     IntToString(rank, RankStr);
     IntToString( TotalPlayers, TotalScore);
     strncpy(MessageBuffer+10,RankStr, strlen(RankStr));
     strncpy( MessageBuffer+15, TotalScore, strlen(TotalScore));

    printf(" send grade message %s \n", MessageBuffer);
    send(socketFd, MessageBuffer, MESSAGE_SIZE, 0);


    printf("Le jeu est en train d'être joué\n");

    // Libérer la mémoire si le serveur est le créateur du jeu
    if (IamLoader == 1) {
        IamLoader = 0;
        for (int i = 0; i < QuestionNumber; i++) {

           /* if (strlen(GlobalQuesMatrix[i]) > 0) {
                printf(" %s\n", GlobalQuesMatrix[i]);
            }*/
            free(Local_Question_Matrix[i]);
        }
        free(Local_Question_Matrix);
    }
    printf("Mémoire libérée proprement\n");
}
 


int get_Flag(char *buffer) {
    char strFlag[3] = {0};  // Tableau pour stocker les deux derniers caractères + '\0'
    int response;

    // Vérifier si l'avant-dernier caractère est '*'
    if (buffer[MESSAGE_SIZE - 2] == '*') {
        // Si oui, on ne prend que le dernier caractère
        strFlag[0] = buffer[MESSAGE_SIZE - 1];
    } else {
        // Sinon, on prend les deux derniers caractères
        strFlag[0] = buffer[MESSAGE_SIZE - 2];
        strFlag[1] = buffer[MESSAGE_SIZE - 1];
    }

    // Convertir la chaîne en entier
    response = atoi(strFlag);

    printf("\nLa valeur retournée par get_Flag est : %d\n", response);

    return response;
}


void Action_Switch(int flag, int* Responseflag, char * MessageBuffer , SOCKET socket, int * Ptr_User_Id , int * Ptr_User_Score, char ** LocalQuestionMatrix ){
    printf(" bien dans do some action \n");
    switch (flag)
    {
    case 1:
         Handle_GameLunch();
        break;
    case 2:
       printf(" option 2");
        break;
    case 3 :       
         Handle_Login(Responseflag, MessageBuffer, Ptr_User_Id, Ptr_User_Score);
         printf(" User Id : %d", *Ptr_User_Id );
        break;
    case 4:
        Handle_SignUp(Responseflag, MessageBuffer);
        break;    
     case 8:
        HandlePlayGame(MessageBuffer, socket, Ptr_User_Id, Ptr_User_Score);
        break; 
    case 11:
         Handle_Check_MatrixLoaded(socket , MessageBuffer);
        break; 
    default:
    printf(" do some action");
        break;
    }

}

//  client handler
DWORD WINAPI ClientHandler(LPVOID clientSocket) {
    int ResponseFlag;
    int UserId=-1;
    int * PtrUserId=&UserId;
    int UserScore=-1;
    int * Ptr_User_Score= &UserScore;
    char ** LocalQuestionMatrix;

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
           int flag = get_Flag(buffer);

           Action_Switch(flag, &ResponseFlag, buffer, clientSock,PtrUserId, Ptr_User_Score, LocalQuestionMatrix);
 
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
