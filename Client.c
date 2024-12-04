#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define BUFFER_SIZE 1024
#define CHAR_SIZE 50
#define MESSAGE_SIZE 22
#define PART_SIZE 10
#define Login_F 3
#define QUESTION_SIZE 510
#define OPTION_SIZE 100

// Structure de l'utilisateur
typedef struct {
    int UserId;
    char nom[PART_SIZE];
    char password[PART_SIZE];
} User;

// Protocole de gestion des messages
typedef enum {
    messageError = 1,
    messageSuccess = 2,
    LoginF = 3,
    NewAccount = 4,
    GiveAnswer = 5
} MessageProtocol;

// Fonction utilitaire pour afficher un titre centralisé avec des motifs
void PrintDecoratedTitle(const char* title) {
    const int consoleWidth = 50; // Largeur de la console pour le centrage
    int titleLength = strlen(title);
    int padding = (consoleWidth - titleLength) / 2;
    printf("\033[1;34m"); // Couleur bleue
    for (int i = 0; i < padding; i++) printf("-");
    printf("%s", title);
    for (int i = 0; i < padding; i++) printf("-");
    printf("\033[0m\n");
}

int getFlag(char *buffer) {
    char charFlag;
    charFlag = *(buffer + MESSAGE_SIZE - 1);
    switch (charFlag) {
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        default:
            return 5;
    }
}

char* CheckUserIdentity(User user, SOCKET socket, char* MessageBuffer) {
    PrintDecoratedTitle("VERIFICATION D'IDENTITE");
    printf("\033[1;32mVoici le message avant l'envoi:\033[0m %s\n", MessageBuffer);
    int SentMessageFd = send(socket, MessageBuffer, MESSAGE_SIZE, 0);
    if (SentMessageFd == SOCKET_ERROR) {
        printf("\033[1;31mErreur d'envoi du message:\033[0m %d\n", WSAGetLastError());
        memset(MessageBuffer, '*', MESSAGE_SIZE);
        MessageBuffer[MESSAGE_SIZE - 1] = messageError;
        return MessageBuffer;
    }

    printf("\033[1;33mEN ATTENTE DE CONFIRMATION D'IDENTITE\033[0m\n");

    int ReceivedMessage = recv(socket, MessageBuffer, MESSAGE_SIZE, 0);
    printf("\033[1;32mMessage reçu\033[0m\n");
    if (ReceivedMessage == SOCKET_ERROR) {
        printf("\033[1;31mErreur de réception du message:\033[0m %d\n", WSAGetLastError());
        memset(MessageBuffer, '*', MESSAGE_SIZE);
        MessageBuffer[MESSAGE_SIZE - 1] = messageError;
        return MessageBuffer;
    }

    int ResponseFlag = getFlag(MessageBuffer);
    if (ResponseFlag != 3) {
        MessageBuffer[MESSAGE_SIZE - 1] = messageError;
        printf("\033[1;31mUN TEL UTILISATEUR N'EXISTE PAS, VEUILLEZ CREER UN COMPTE\033[0m\n");
        return MessageBuffer;
    }
    MessageBuffer[MESSAGE_SIZE - 1] = messageSuccess;
    return MessageBuffer;
}

char* CreateUser(SOCKET socket, char* MessageBuffer) {
    User myuser;
    PrintDecoratedTitle("CREATION D'UN NOUVEAU COMPTE");

    printf("\033[1;36mCREER UN NOM D'UTILISATEUR:\033[0m ");
    scanf("%9s", myuser.nom);

    printf("\033[1;36mCREER UN MOT DE PASSE:\033[0m ");
    scanf("%9s", myuser.password);

    memset(MessageBuffer, '*', MESSAGE_SIZE);
    strncpy(MessageBuffer, myuser.nom, strlen(myuser.nom));
    strncpy(MessageBuffer + PART_SIZE - 1, myuser.password, strlen(myuser.password));
    strncpy(MessageBuffer + MESSAGE_SIZE - 1, "4", 1);

    if (send(socket, MessageBuffer, MESSAGE_SIZE, 0) == SOCKET_ERROR) {
        printf("\033[1;31mErreur d'envoi:\033[0m %d\n", WSAGetLastError());
        memset(MessageBuffer, '*', MESSAGE_SIZE);
        MessageBuffer[MESSAGE_SIZE - 1] = messageError;
        return MessageBuffer;
    }

    printf("\033[1;33mEN ATTENTE DE LA CREATION DU COMPTE\033[0m\n");

    memset(MessageBuffer, '*', MESSAGE_SIZE);
    int ReceivedMessage = recv(socket, MessageBuffer, MESSAGE_SIZE, 0);
    if (ReceivedMessage <= 0) {
        printf("\033[1;31mErreur de réception ou connexion fermée:\033[0m %d\n", WSAGetLastError());
        memset(MessageBuffer, '*', MESSAGE_SIZE);
        MessageBuffer[MESSAGE_SIZE - 1] = messageError;
        return MessageBuffer;
    }

    int ResponseFlag = getFlag(MessageBuffer);
    if (ResponseFlag != 4) {
        printf("\033[1;31mErreur lors de la création du compte.\033[0m\n");
        memset(MessageBuffer, '*', MESSAGE_SIZE);
        MessageBuffer[MESSAGE_SIZE - 1] = messageError;
        return MessageBuffer;
    }

    memset(MessageBuffer, '*', MESSAGE_SIZE);
    MessageBuffer[MESSAGE_SIZE - 1] = messageSuccess;
    printf("\033[1;32mCOMPTE CREE AVEC SUCCES!\033[0m\n");
    return MessageBuffer;
}

void Login(int *Islog, SOCKET socket, char* MessageBuffer) {
    User myuser;
    myuser.UserId = 0;
    int userAction;

    PrintDecoratedTitle("LOGIN");

    while (1) {
        printf("\033[1;36mENTREZ VOS IDENTIFIANTS POUR VOUS CONNECTER\033[0m\n");
        printf("\033[1;36mENTREZ VOTRE NOM D'UTILISATEUR:\033[0m ");
        scanf("%9s", myuser.nom);
        printf("\033[1;36mENTREZ VOTRE MOT DE PASSE:\033[0m ");
        scanf("%9s", myuser.password);

        memset(MessageBuffer, '*', MESSAGE_SIZE);
        strncpy(MessageBuffer, myuser.nom, strlen(myuser.nom));
        strncpy(MessageBuffer + PART_SIZE - 1, myuser.password, strlen(myuser.password));
        strncpy(MessageBuffer + MESSAGE_SIZE - 1, "3", 1);

        CheckUserIdentity(myuser, socket, MessageBuffer);

        if (MessageBuffer[MESSAGE_SIZE - 1] == messageSuccess) {
            *Islog = 1;
            PrintDecoratedTitle("CONNEXION REUSSIE");
            break;
        }

        printf("\033[1;31mLE LOGIN A ECHOUE, ERREUR\033[0m: %s\n", myuser.nom);
        printf("\033[1;33m1 = CREER UN NOUVEAU COMPTE, 2 = QUITTER, AUTRE = RESSAYER\033[0m\n");
        scanf("%d", &userAction);
        if (userAction == 1) {
            CreateUser(socket, MessageBuffer);
        } else if (userAction == 2) {
            break;
        }
    }
}
// Ma fonction pour  retourner le vrai choi
char* ChosenAnswer(char* KeyboardChar, char* opt1, char* opt2, char* opt3, char* opt4)
{
    char SwCkeck= KeyboardChar[0];
    switch (SwCkeck)
    {
    case 'A':
    case 'a':
        return opt1;
    case 'B':
    case 'b':
        return opt2;
    case 'C':
    case 'c':
        return opt3;
    case 'D':
    case 'd':
        return opt4;
    default:
        printf(" VOTRE ENTREE N'EST PAS VALIDE. L'OPTION 1 SERA RENVOYEE PAR DEFAUT.\n");
        return opt1;
    }
}


 void TestGame(SOCKET socket, char* MessageBuffer) {
    // variable pour les boucles
    int i;
    // initialisation du buffer qui va stocker le message des questions
    char QuesMessageBuffer[QUESTION_SIZE] = {0};

    // initialisation du message buffer
    memset(MessageBuffer, '*', MESSAGE_SIZE);
    char QuizQuestion[5];
// "\033[1;33m1 = CREER UN NOUVEAU COMPTE, 2 = QUITTER, AUTRE = RESSAYER\033[0m\n"
    printf("\033[1;33m1 COMBIEN DE QUESTIONS VOULEZ-VOUS GÉNÉRER DANS LE QUIZ \033[0m\n");
    scanf("%s", QuizQuestion);

    // Modifie le message buffer pour qu'il envoie le nombre de questions souhaitées
    for(i = 0; i < strlen(QuizQuestion); i++) {
        MessageBuffer[i] = QuizQuestion[i];
    }

    MessageBuffer[MESSAGE_SIZE - 1] = '8';  // Indiquer que c'est une demande de création de jeu
    int SentMessageFd = send(socket, MessageBuffer, MESSAGE_SIZE, 0);
    if (SentMessageFd == SOCKET_ERROR) {
        printf("\033[1;31mErreur d'envoi du message:\033[0m %d\n", WSAGetLastError());
        return;
    }

    // Attendre la réponse du serveur concernant la création du jeu
    int ReceiveMessageFd = recv(socket, MessageBuffer, MESSAGE_SIZE, 0);
    if (ReceiveMessageFd == SOCKET_ERROR) {
        printf("\033[1;31mErreur lors de la réception du message de confirmation de création de jeu :\033[0m %d\n", WSAGetLastError());
        return;
    }

    // Vérifier si le jeu a été correctement créé
    if (MessageBuffer[0] != '1') {
        printf("ERREUR LORS DE LA PRÉPARATION DU JEU\n");
        return;
    }

    // Attente des questions et envoi des réponses
    int RecvQuestionFd, SentQuestion;
    int QuestionNumber = atoi(QuizQuestion);
    char MyChoice[4];
   

    for(i = 0; i < QuestionNumber; i++) {
         char bufOpt1[OPTION_SIZE]={0};
         char bufOpt2[OPTION_SIZE]={0};
         char bufOpt3[OPTION_SIZE]={0};
         char bufOpt4[OPTION_SIZE]={0};
        // Réception de la question
        RecvQuestionFd = recv(socket, QuesMessageBuffer, QUESTION_SIZE, 0);
        if (RecvQuestionFd == SOCKET_ERROR) {
            printf("\033[1;31mErreur lors de la réception de la question numéro :%d, ERREUR :\033[0m %d\n", i, WSAGetLastError());
            return;
        } else {
            // Afficher les options pour l'utilisateur
            // \033[0m
            printf("\033[1;33m Question %d: %s \033[0m \n", i + 1, QuesMessageBuffer);
            printf("\033[1;34m Choisissez votre réponse (A-D) : 033[0m\n");  
            // on affiche et copie dans le buffer l'option1

            printf(" \033[1;33m A. %s \033[0m \n", QuesMessageBuffer + OPTION_SIZE);
            strncpy(bufOpt1,QuesMessageBuffer + OPTION_SIZE,OPTION_SIZE );

            // on affiche et copie dans le buffer l'option2
            printf(" \033[1;33m B. %s \033[0m  \n", QuesMessageBuffer + 2 * OPTION_SIZE);
            strncpy(bufOpt2,QuesMessageBuffer + 2*OPTION_SIZE,OPTION_SIZE );

            // on affiche et copie dans le buffer l'option3
            printf(" \033[1;33m C. %s \033[0m \n", QuesMessageBuffer + 3 * OPTION_SIZE);
            strncpy(bufOpt3,QuesMessageBuffer + 3*OPTION_SIZE,OPTION_SIZE );

             // on affiche et copie dans le buffer l'option4
            printf(" \033[1;33m D. %s \033[0m \n", QuesMessageBuffer + 4 * OPTION_SIZE);
            strncpy(bufOpt4,QuesMessageBuffer + 4*OPTION_SIZE,OPTION_SIZE );

            // Entrée de la réponse
            scanf("%1s", MyChoice);

            // Préparer la réponse
            memset(QuesMessageBuffer, 0, QUESTION_SIZE);
            // je charge ma reponse dans le buffer
            memset( QuesMessageBuffer, 0, QUESTION_SIZE);
            strncpy(QuesMessageBuffer,  ChosenAnswer(MyChoice, bufOpt1, bufOpt2, bufOpt3, bufOpt4  ), OPTION_SIZE );

            QuesMessageBuffer[QUESTION_SIZE - 1] = '9';  // Flag de fin de réponse

            // Envoyer la réponse
            SentQuestion = send(socket, QuesMessageBuffer, QUESTION_SIZE, 0);
            if (SentQuestion == SOCKET_ERROR) {
                printf("\033[1;31mErreur lors de l'envoi de la réponse :\033[0m %d\n", WSAGetLastError());
                return;
            }
        }
    }
}


int main() {
    WSADATA wsadata;
    int Islog = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("\033[1;31mErreur d'initialisation:\033[0m %d\n", WSAGetLastError());
        exit(1);
    }

    SOCKET SocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFd == INVALID_SOCKET) {
        printf("\033[1;31mErreur de création du socket:\033[0m %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    struct sockaddr_in SocketAddress;
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(5600);

    int inetReturnCode = inet_pton(AF_INET, "192.168.1.133", &SocketAddress.sin_addr);
    if (inetReturnCode <= 0) {
        printf("\033[1;31mErreur d'adresse IP.\033[0m\n");
        closesocket(SocketFd);
        WSACleanup();
        exit(1);
    }

    PrintDecoratedTitle("CONNEXION AU SERVEUR");

    if (connect(SocketFd, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress)) == SOCKET_ERROR) {
        printf("\033[1;31mErreur de connexion:\033[0m %d\n", WSAGetLastError());
        closesocket(SocketFd);
        WSACleanup();
        exit(1);
    }

    printf("\033[1;32mConnexion acceptée.\033[0m\n");

    char MessageBuffer[MESSAGE_SIZE];

    Login(&Islog, SocketFd, MessageBuffer);

    if (Islog) {

        PrintDecoratedTitle("BONNE CONTINUATION \n");
         PrintDecoratedTitle(" YOUR GAME HAS STARTED \n");
        TestGame(SocketFd, MessageBuffer);
    }

    // printf(" je vais tester le jeu \n");
    // TestGame(SocketFd, MessageBuffer);

    closesocket(SocketFd);
    WSACleanup();

    return 0;
}
