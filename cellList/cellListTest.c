#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cellList.h"

int main(){
    char *separator = "===========================================================================";
    printf("%s\n\n", separator);
	List *l = initList();

    for(int i = 0; i < 15; i++){
        char str_int[2];
        sprintf(str_int, "%d", i);
        Cell *c = buildCell(str_int);
        insertFirst(l, c);
    }


    printf("Vérification de la création de liste ainsi que ces cellules :\n   -> ");
        Cell *c_list = *l;
        while(c_list){
            printf("%s ", ctos(c_list));
            c_list = c_list->next;
        }
    printf("\nRésultat attendu : 14 13 12 ... 0 (car insertFirst, et pas insertLast)\n");


    printf("\n%s\n", separator);


    printf("\nVérification de la fonction ltos()\n   -> ");
        char *str_ltos = ltos(l);
        printf("%s\n", str_ltos);
        free(str_ltos);
    printf("Résultat attendu : 14|13|12|...|1|0\n");


    printf("\n%s\n", separator);


    printf("\nVérification de la fonction listGet()\n   -> ");
        Cell *c_get = listGet(l, 3);
        if (c_get != NULL)
            printf("%s\n", ctos(c_get));
        else
            printf("La cellule n'existe pas/est vide !\n");


    printf("\n%s\n", separator);


    printf("\nVérification de la fonction searchList()\n   -> ");
        Cell *c_search = searchList(l, "15");
        if (c_search != NULL)
            printf("%s\n", ctos(c_search));
        else
            printf("L'élement n'est pas dans la liste !\n");


    printf("\n%s\n", separator);


    printf("\nVérification de la fonction stol()\n");
        char str[] = "Ceci_est_un_test_de_stol";
        char delim[] = "_";
        List *l_stol = stol(str, delim);
        Cell *c_stol = *l_stol;
        int i = 1;
        while(c_stol){
            printf("   -> Celule %d : %s\n", i, c_stol->data);
            c_stol = c_stol->next;
            i++;
        }
        

    printf("\n%s\n", separator);


    // A vérifier manuellement dans le fichier texte test_ltof.txt
    ltof(l, "test_ltof.txt");

    printf("\nVérification de la fonction ftol :\n");
    List *l_ftol = ftol("test_ftol.txt");
    Cell *c_ftol = *l_ftol;
    i = 1;
    while(c_ftol){
        printf("   -> Cellule %d : %s\n", i, ctos(c_ftol));
        c_ftol = c_ftol->next;
    }


    printf("\n%s\n", separator);


    freeList(l_ftol);
    freeList(l_stol);
    freeList(l);


    //===============================================================================


    printf("\n");

    List* test = listdir("/home/deniz");
	ltof(test, "test_listdir.txt");

	printf("%d\n", file_exists("cellLt.c"));

	cp("copie.txt", "cellList.c");

    char *path = hashToPath("flkaejfldskjgldakgj");
	printf("%s\n", path);

	blobFile("copie.txt");
    printf("\n");
	
    free(path);
	free(test);

	return 0;
}