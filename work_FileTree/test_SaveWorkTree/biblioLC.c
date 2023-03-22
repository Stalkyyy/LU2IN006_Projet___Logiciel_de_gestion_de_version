#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblioLC.h"

//====================================================================


Livre* creer_livre(int num, char* titre, char* auteur){
    Livre *L = malloc(sizeof(Livre));
    if (L == NULL) exit(1);

    L->num = num;
    L->titre = strdup(titre);
    L->auteur = strdup(auteur);
    L->suiv = NULL;

    return L;
}


void liberer_livre(Livre *L){
    free(L->titre);
    free(L->auteur);
    free(L);
}


void afficher_livre(Livre *L){
    if (L != NULL)
        printf("%d %s %s\n", L->num, L->titre, L->auteur);
}


//====================================================================


Biblio* creer_biblio(){
    Biblio* b = malloc(sizeof(Biblio));
    if (b == NULL) exit(0);

    b->L = NULL;
    return b;
}


void liberer_biblio(Biblio* b){
    Livre *liv = b->L;

    while(liv != NULL){
        Livre* tmp = liv->suiv;
        liberer_livre(liv);
        liv = tmp;
    }

    free(b);
}


void inserer_en_tete(Biblio* b, int num, char* titre, char* auteur){
    Livre* newL = creer_livre(num, titre, auteur);
    newL->suiv = b->L;
    b->L = newL;
}


void afficher_biblio(Biblio* b){
    Livre *L = b->L;
    while(L != NULL){
        afficher_livre(L);
        L = L->suiv;
    }
}


Livre* recherche_num(Biblio* b, int n){
    Livre *L = b->L;

    while(L != NULL){
        if (L->num == n) 
            return L;
        L = L->suiv;
    }

    return NULL;
}


Livre* recherche_titre(Biblio* b, char* titre){
    Livre *L = b->L;

    while(L != NULL){
        if(strcmp(L->titre, titre) == 0)
            return L;
        L = L->suiv;
    }

    return NULL;
}


Biblio* recherche_auteur(Biblio* b, char* auteur){
    Biblio* res = creer_biblio();
    Livre *L = b->L;

    while(L != NULL){
        if(strcmp(L->auteur, auteur) == 0)
            inserer_en_tete(res, L->num, L->titre, L->auteur);
        L = L->suiv;
    }

    return res;
}


int supression_ouvrage(Biblio* b, int num, char* titre, char* auteur){
    Livre *L = b->L;

    if (L == NULL) return 1;

    if (L->num == num && strcmp(L->titre, titre) == 0 && strcmp(L->auteur, auteur) == 0){
        b->L = L->suiv;
        liberer_livre(L);
    }

    Livre *tmp = L;
    L = L->suiv;

    while(L != NULL){
        if (L->num == num && strcmp(L->titre, titre) == 0 && strcmp(L->auteur, auteur) == 0){
            tmp->suiv = L->suiv;
            liberer_livre(L);
            return 0;
        } else {
            tmp = L;
            L = L->suiv;
        }
    }

    return 1;
}


void fusion_biblio(Biblio *b1, Biblio *b2){
    Livre *L2 = b2->L;

    while(L2 != NULL){
        inserer_en_tete(b1, L2->num, L2->titre, L2->auteur);
        L2 = L2->suiv;
    }

    liberer_biblio(b2);
}


Biblio* recherche_doublon(Biblio* b){
    Biblio *doublons = creer_biblio();

    Livre *L1 = b->L;
    while(L1){
        Livre *L2 = b->L;
        while(L2){
            if (L1 != L2 && strcmp(L1->titre, L2->titre) == 0 && strcmp(L1->auteur, L2->auteur) == 0){
                inserer_en_tete(doublons, L1->num, L1->titre, L1->auteur);
                break;
            }

            L2 = L2->suiv;
        }

        L1 = L1->suiv;
    }

    return doublons;
}
