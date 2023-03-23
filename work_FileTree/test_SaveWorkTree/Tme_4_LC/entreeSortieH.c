#include <stdio.h>
#include <stdlib.h>
#include "entreeSortieH.h"

BiblioH* charger_n_entrees(char* nomfic, int n){
    FILE *f = fopen(nomfic, "r");
    if (f == NULL) exit(1);

    BiblioH* b = creer_biblio(n);
    int num; char titre[255]; char auteur[255];

    int i = 0;

    // EOF != fscanf(f, "%d %s %s", &num, titre, auteur) permet de vérifier si notre n n'est pas plus grand que le nombre de lignes du fichier. S'il l'est, on arrête la boucle.
    while(i < n && EOF != fscanf(f, "%d %s %s", &num, titre, auteur)){
        inserer(b, num, titre, auteur);
        i++;
    }

    fclose(f);
    return b;
}


void enregistrer_biblio(BiblioH *b, char* nomfic){
    FILE *f = fopen(nomfic, "w");
    if (f == NULL) exit(1);

    int i = 0;
    while(i < b->m){
        LivreH *l = b->T[i];
        while(l != NULL){
            fprintf(f, "%d %s %s\n", l->num, l->titre, l->auteur);
            l = l->suiv;
        }
        i++;
    }

    fclose(f);
}