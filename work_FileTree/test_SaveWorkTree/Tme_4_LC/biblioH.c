#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "biblioH.h"

int fonctionClef(char *auteur){
    int i = 0;
    int res = 0;
    while(auteur[i] != '\0'){
        res += auteur[i];
        i++;
    }

    return res;
}


LivreH* creer_livre(int num, char* titre, char* auteur){
    LivreH* l = (LivreH*)malloc(sizeof(LivreH));
    if (l == NULL)
        exit(1);

    l->clef = fonctionClef(auteur);

    l->num = num;

    l->titre = strdup(titre);
    if (l->titre == NULL)
        exit(1);

    l->auteur = strdup(auteur);
    if (l->auteur == NULL)
        exit(1);

    l->suiv = NULL;
    return l;
}


void liberer_livre(LivreH* l){
    if (l != NULL){
        free(l->titre);
        free(l->auteur);
        free(l);
    }
}


BiblioH* creer_biblio(int m){
    BiblioH* b = (BiblioH*)malloc(sizeof(BiblioH));
    if (b == NULL)
        exit(1);

    b->nE = 0;
    b->m = m;

    b->T = (LivreH**)malloc(sizeof(LivreH*)*m);
    if(b->T == NULL)
        exit(1);

    return b;
}


void liberer_biblio(BiblioH* b){
    int i = 0;
    while(i < b->m){
        LivreH* l = b->T[i];
        LivreH* temp;
        while(l != NULL){
            temp = l->suiv;
            liberer_livre(l);
            l = temp;
        }
        i++;
    }

    free(b->T);
    free(b);
}


int fonctionHachage(int cle, int m){
    double A = (sqrt(5)-1.0)/2.0;
    double kA = (double)cle * A;

    return floor(m * (kA - floor(kA)));
}


void inserer(BiblioH* b, int num, char* titre, char* auteur){
    int index = fonctionHachage(fonctionClef(auteur), b->m);

    LivreH* l = creer_livre(num, titre, auteur);
    l->suiv = b->T[index];
    b->T[index] = l;

    b->nE++;
}


void afficher_livre(LivreH* l){
    if (l != NULL)
        printf("%d %s %s\n", l->num, l->titre, l->auteur);
}


void afficher_biblio(BiblioH* b){
    int i = 0;
    while(i < b->m){
        LivreH* l = b->T[i];
        while(l){
            afficher_livre(l);
            l = l->suiv;
        }
        i++;
    }
}


LivreH* recherche_num(BiblioH* b, int num){
    int i = 0;
    while(i < b->m){
        LivreH* l = b->T[i];
        while(l){
            if (l->num == num)
                return l;
            l = l->suiv;
        }
        i++;
    }

    return NULL;
}


LivreH* recherche_titre(BiblioH* b, char* titre){
    int i = 0;
    while(i < b->m){
        LivreH* l = b->T[i];
        while(l){
            if (strcmp(l->titre, titre) == 0)
                return l;
            l = l->suiv;
        }
        i++;
    }

    return NULL;
}

BiblioH* recherche_auteur(BiblioH* b, char* auteur){
    BiblioH* res = creer_biblio(b->m);
    int index = fonctionHachage(fonctionClef(auteur), b->m);

    LivreH* l = b->T[index];
    while(l){
        if(strcmp(l->auteur, auteur) == 0)
            inserer(res, l->num, l->titre, l->auteur);
        l = l->suiv;
    }

    return res;
}