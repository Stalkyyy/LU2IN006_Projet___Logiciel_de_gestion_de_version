#ifndef _BIBLIOH_H_
#define _BIBLIOH_H_

typedef struct livreH{
    int clef;
    int num;
    char* titre;
    char* auteur;
    struct livreH *suiv;
} LivreH;

typedef struct table{
    int nE;
    int m;
    LivreH** T;
} BiblioH;


int fonctionClef(char *auteur);

LivreH* creer_livre(int num, char* titre, char* auteur);
void liberer_livre(LivreH* l);
BiblioH* creer_biblio(int m);
void liberer_biblio(BiblioH* b);

int fonctionHachage(int cle, int m);

void inserer(BiblioH* b, int num, char* titre, char* auteur);

void afficher_livre(LivreH* l);
void afficher_biblio(BiblioH* b);

LivreH* recherche_num(BiblioH* b, int num);
LivreH* recherche_titre(BiblioH* b, char* titre);
BiblioH* recherche_auteur(BiblioH* b, char* auteur);

#endif