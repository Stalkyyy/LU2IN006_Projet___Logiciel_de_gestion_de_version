#ifndef _BIBLIOLC_H_
#define _BIBLIOLC_H_

typedef struct livre{
    int num;
    char *titre;
    char *auteur;
    struct livre *suiv;
} Livre;

typedef struct{
    Livre* L;
} Biblio;

Livre* creer_livre(int num, char* titre, char* auteur);
void liberer_livre(Livre* L);
void afficher_livre(Livre *L);

Biblio* creer_biblio();
void liberer_biblio(Biblio *b);
void inserer_en_tete(Biblio *b, int num, char* titre, char* auteur);
void afficher_biblio(Biblio *b);
Livre* recherche_num(Biblio* b, int n);
Livre* recherche_titre(Biblio* b, char* titre);
Biblio* recherche_auteur(Biblio* b, char* auteur);
int supression_ouvrage(Biblio* b, int num, char* titre, char* auteur);
void fusion_biblio(Biblio* b1, Biblio* b2);
Biblio* recherche_doublon(Biblio* b);

#endif