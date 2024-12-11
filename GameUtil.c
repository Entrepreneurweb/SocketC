#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include  "GameUtil.h"

// Fonction pour ajouter des questions
int Add_Question() {
    FILE *Question_FileStream = fopen("Doc.txt", "a+");
    FILE *Id_FileStream = fopen("Id.txt", "r+");
    if (Question_FileStream == NULL || Id_FileStream == NULL) {
        printf("ERREUR D'OUVERTURE DE FICHIER\n");
        return -2;
    }

    char buffer[170];
    char Question_Buffer[100];
    char Option1_Buffer[100];
    char Option2_Buffer[100];
    char Option3_Buffer[100];
    char Option4_Buffer[100];
    char Correct_Option_Buffer[100];
    char Id_Text[10];
    int Cur_Id = 0;

    // Lire l'ID actuel
    while (fgets(buffer, sizeof(buffer), Id_FileStream)) {
        sscanf(buffer, "ID:%s", Id_Text);
        Cur_Id = atoi(Id_Text);
    }

    // Entrée utilisateur
    int Question_Number;
    printf("ENTREZ LE NOMBRE DE QUESTIONS A AJOUTER : ");
    scanf("%d", &Question_Number);
    getchar(); // Consomme le '\n' résiduel

    for (int i = 0; i < Question_Number; i++) {
        printf("\nENTREZ LA QUESTION NUMERO %d : ", i + 1);
        fgets(Question_Buffer, sizeof(Question_Buffer), stdin);
        Question_Buffer[strcspn(Question_Buffer, "\n")] = '\0';

        printf("ENTREZ LA REPONSE NUMERO 1 : ");
        fgets(Option1_Buffer, sizeof(Option1_Buffer), stdin);
        Option1_Buffer[strcspn(Option1_Buffer, "\n")] = '\0';

        printf("ENTREZ LA REPONSE NUMERO 2 : ");
        fgets(Option2_Buffer, sizeof(Option2_Buffer), stdin);
        Option2_Buffer[strcspn(Option2_Buffer, "\n")] = '\0';

        printf("ENTREZ LA REPONSE NUMERO 3 : ");
        fgets(Option3_Buffer, sizeof(Option3_Buffer), stdin);
        Option3_Buffer[strcspn(Option3_Buffer, "\n")] = '\0';

        printf("ENTREZ LA REPONSE NUMERO 4 : ");
        fgets(Option4_Buffer, sizeof(Option4_Buffer), stdin);
        Option4_Buffer[strcspn(Option4_Buffer, "\n")] = '\0';

        printf("ENTREZ LA REPONSE CORRECTE : ");
        fgets(Correct_Option_Buffer, sizeof(Correct_Option_Buffer), stdin);
        Correct_Option_Buffer[strcspn(Correct_Option_Buffer, "\n")] = '\0';

        // Écrire dans le fichier des questions
        fprintf(Question_FileStream, "ID:%d QUESTION:%s OPTION1:%s OPTION2:%s OPTION3:%s OPTION4:%s CORRECTOPTION:%s\n",
                Cur_Id, Question_Buffer, Option1_Buffer, Option2_Buffer, Option3_Buffer, Option4_Buffer, Correct_Option_Buffer);

        // Mettre à jour l'ID
        Cur_Id++;
    }

    // Mettre à jour le fichier Id.txt
    freopen("Id.txt", "w+", Id_FileStream);
    fprintf(Id_FileStream, "ID:%d", Cur_Id);

    printf("LES QUESTIONS ONT ÉTÉ AJOUTÉES AVEC SUCCÈS\n");
    fclose(Question_FileStream);
    fclose(Id_FileStream);

    return 0;
}

// Fonction pour préparer un tableau de questions
char **Prepared_Ques_Arr(int QuestionNumber) {
    // Correction : Utilisation correcte d'un tableau de pointeurs
    char **QuestionMatrix = (char**)malloc(QuestionNumber * sizeof(char*));
    if (QuestionMatrix == NULL) {
        printf("Erreur d'allocation mémoire pour QuestionMatrix.\n");
        return NULL;
    }

    for (int i = 0; i < QuestionNumber; i++) {
        QuestionMatrix[i] = (char*)malloc(500 * sizeof(char));
        if (QuestionMatrix[i] == NULL) {
            printf("Erreur d'allocation mémoire pour QuestionMatrix[%d].\n", i);
            for (int j = 0; j < i; j++) {
                free(QuestionMatrix[j]); // Libérer les allocations précédentes
            }
            free(QuestionMatrix);
            return NULL;
        }
    }

    return QuestionMatrix;
}

// Générer des nombres aléatoires uniques
int *Random_Without_Reps(int min, int max, int sizeOfArray, int *AllreadyGenerated) {
    int OurRandNumber;
    int isDuplicate;
srand(time(NULL)); 
    memset(AllreadyGenerated, -1, sizeOfArray * sizeof(int)); // Initialisation

    for (int i = 0; i < sizeOfArray; i++) {
        isDuplicate = 1;

        while (isDuplicate) {
            OurRandNumber = rand() % (max - min + 1) + min;
            isDuplicate = 0;

            for (int j = 0; j < i; j++) {
                if (AllreadyGenerated[j] == OurRandNumber) {
                    isDuplicate = 1;
                    break;
                }
            }
        }

        AllreadyGenerated[i] = OurRandNumber;
        printf("Unique random number: %d\n", OurRandNumber);
    }

    return AllreadyGenerated;
}

// Vérifier si une valeur est dans un tableau
int checkIfInRandQues(int *array, int arraySize, int CheckVal) {
    int Result = 0;
    for (int i = 0; i < arraySize; i++) {
        if (array[i] == CheckVal) {
            Result = 1;
            break;
        }
    }
    return Result;
}

// Préparer un quiz
void Prepare_Quiz(char ** QuestionMatrix , int* questionNumber) {
    char ReaderBuffer[500];
  //  printf("ENTER THE NUMBER OF QUESTION IN THE QUIZZ \n");
   // int questionNumber;
   // scanf("%d", questionNumber);

    int *Rand_Integer_Arr = (int *)malloc(sizeof(int) * ( *(questionNumber)));
    if (Rand_Integer_Arr == NULL) {
        printf("Erreur d'allocation mémoire pour Rand_Integer_Arr.\n");
        return;
    }
    Random_Without_Reps(0, 50, (*questionNumber), Rand_Integer_Arr);

    //char **QuestionMatrix = Prepared_Ques_Arr(Question_Number);
    if (QuestionMatrix == NULL) {
        free(Rand_Integer_Arr);
        return;
    }

    FILE *Question_FileStream = fopen("Doc.txt", "r");
    if (Question_FileStream == NULL) {
        printf("Erreur d'ouverture du fichier Doc.txt.\n");
        free(Rand_Integer_Arr);
        
        for (int i = 0; i < (*questionNumber); i++) {
            free(QuestionMatrix[i]);
        }
        free(QuestionMatrix);
        return;
    }

    int counter = 0;
    while (fgets(ReaderBuffer, sizeof(ReaderBuffer), Question_FileStream) && counter < (*questionNumber)) {
        // Extraire l'ID de la question (dans le format "ID:1 QUESTION:...")
        int questionID = -1;
        sscanf(ReaderBuffer, "ID:%d", &questionID);

        // Vérifier si l'ID de la question est dans le tableau Rand_Integer_Arr
        if (checkIfInRandQues(Rand_Integer_Arr, (*questionNumber), questionID)) {
            strcpy(QuestionMatrix[counter], ReaderBuffer);
            counter++;
        }
    }

    fclose(Question_FileStream);
/*
    for (int i = 0; i < (*questionNumber); i++) {
        if (strlen(QuestionMatrix[i]) > 0) {
            printf(" %s\n", QuestionMatrix[i]);
        }
        free(QuestionMatrix[i]); // Libérer chaque ligne
    }

    free(QuestionMatrix);*/
    free(Rand_Integer_Arr);
}

void TakeQuiz(char ** QuestionMatrix)
{
    
}