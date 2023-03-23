#include <stdio.h>
#include <stdlib.h>
#include "entreeSortieLC.h"

Biblio* charger_n_entrees(char* nomfic, int n){
    FILE *f = fopen(nomfic, "r");
    if (f == NULL) exit(1);

    Biblio* b = creer_biblio();
    int num; char titre[255]; char auteur[255];

    int i = 0;

    // EOF != fscanf(f, "%d %s %s", &num, titre, auteur) permet de vérifier si notre n n'est pas plus grand que le nombre de lignes du fichier. S'il l'est, on arrête la boucle.
    while(i < n && EOF != fscanf(f, "%d %s %s", &num, titre, auteur)){
        inserer_en_tete(b, num, titre, auteur);
        i++;
    }

    fclose(f);
    return b;
}


void enregistrer_biblio(Biblio *b, char* nomfic){
    FILE *f = fopen(nomfic, "w");
    if (f == NULL) exit(1);

    Livre *L = b->L;

    while(L != NULL){
        fprintf(f, "%d %s %s\n", L->num, L->titre, L->auteur);
        L = L->suiv;
    }

    fclose(f);
}