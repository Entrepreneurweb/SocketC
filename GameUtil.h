#ifndef GAMEUTIL_H
#define GAMEUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Déclaration des constantes
#define MAX_QUESTION_LENGTH 500
#define MAX_OPTION_LENGTH 100

// Déclaration des fonctions

/**
 * Ajoute des questions dans les fichiers de données.
 * return 0 en cas de succès, un code d'erreur sinon.
 */
int Add_Question();

/**
 * Prépare un tableau dynamique pour stocker des questions.
 * param questionNumber Nombre de questions à stocker.
 * return Un pointeur vers le tableau de chaînes de caractères, ou NULL en cas d'erreur.
 */
char** Prepared_Ques_Arr(int questionNumber);

/**
 * Génère un tableau de nombres aléatoires uniques.
 * param min Valeur minimale.
 * param max Valeur maximale.
 * param sizeOfArray Taille du tableau généré.
 * param alreadyGenerated Tableau où stocker les nombres uniques.
 * return Un pointeur vers le tableau des nombres uniques.
 */
int* Random_Without_Reps(int min, int max, int sizeOfArray, int* alreadyGenerated);

/**
 * Vérifie si une valeur est présente dans un tableau.
 * param array Tableau dans lequel rechercher.
 * param arraySize Taille du tableau.
 * param checkVal Valeur à vérifier.
 * return 1 si la valeur est trouvée, 0 sinon.
 */
int checkIfInRandQues(int* array, int arraySize, int checkVal);

/**
 * Prépare un quiz en sélectionnant des questions aléatoires.
 * param QuestionMatrix Tableau dynamique pour stocker les questions sélectionnées.
 * param questionNumber Pointeur vers le nombre de questions à inclure dans le quiz.
 */
void Prepare_Quiz(char** QuestionMatrix, int* questionNumber);

/**
 * Lance le quiz pour l'utilisateur.
 * param QuestionMatrix Tableau contenant les questions préparées.
 */
void TakeQuiz(char** QuestionMatrix);

#endif // QUIZ_MODULE_H
