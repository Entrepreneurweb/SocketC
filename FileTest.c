#include <stdio.h>
#include <string.h>

#define PART_SIZE 10        // Taille maximale pour le nom et le mot de passe
#define MESSAGE_SIZE 20     // Taille maximale pour le message total (nom + mot de passe)

// Fonction d'inscription
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

// Fonction de connexion
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

// Fonction principale pour tester l'inscription et la connexion
int main() {
    int responseFlag = 0;
char SignupMessage[] = "exo*******test******"; // exo*******test******
    
HandleSignUp(&responseFlag , SignupMessage);


    // Test de la connexion avec des bonnes informations
    //char loginMessage[] = "exo******test*******";  // Format: nom******motdepasse*******
    printf("\nTest de connexion (bonne information):\n");
    HandleLogin(&responseFlag, SignupMessage);
    printf("Réponse de connexion: %d\n", responseFlag);

    
   

    return 0;
}
