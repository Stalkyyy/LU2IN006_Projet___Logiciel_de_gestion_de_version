#include <stdio.h>
#include <stdlib.h>
#include "biblioLC.h"
#include "entreeSortieLC.h"

void menu(){
    printf("Choississez votre action :\n");
    printf("\tSortie du programme (0)\n");
    printf("\tAffichage (1)\n");
    printf("\tInserer un ouvrage (2)\n");
    printf("\tSupprimer un ouvrage (3)\n");
    printf("\tRecherche par numéro (4)\n");
    printf("\tRecherche par titre (5)\n");
    printf("\tRecherche par auteur (6)\n");
    printf("\tRecherche de boublons (7)\n\n");
}


int main(int argc, char** argv){
    if (argc != 3){
        printf("Mauvaise utilisation !\n");
        printf("Usage : %s <nomfic> <nombre de lignes>\n", argv[0]);
        exit(1);
    }

    char *nomfic = argv[1];
    int n = atoi(argv[2]);

    Biblio* b = charger_n_entrees(nomfic, n);

    int rep;
    int num; char titre[256]; char auteur[256];
    do{
        printf("\n"); menu(); printf("Votre choix : "); scanf("%d", &rep);

        switch (rep)
        {
        case 1 :
            printf("\nAffichage :\n");
            afficher_biblio(b);
            break;
        

        case 2 :
            printf("\nVeuillez écrire le numéro, le titre et l'auteur de l'ouvrage.\n   => ");
            if (scanf("%d %s %s", &num, titre, auteur) == 3){
                inserer_en_tete(b, num, titre, auteur);
                printf("Ajout fait.\n");
            } else {
                printf("Erreur format.\n");
            }
            break;


        case 3 :
            printf("\nVeuillez écrire le numéro, le titre et l'auteur de l'ouvrage.\n   => ");
            if (scanf("%d %s %s", &num, titre, auteur) == 3){
                if (supression_ouvrage(b, num, titre, auteur) == 0)
                    printf("Suppression réalisée.\n");
                else 
                    printf("Ouvrage non trouvé.\n");
            } else {
                printf("Erreur format.\n");
            }
            break;
        

        case 4 :
            printf("\nVeuillez écrire votre numéro d'ouvrage : "); 
            if (scanf("%d", &num) == 1){
                Livre* L = recherche_num(b, num);

                if (L != NULL){
                    printf("\t=> ");
                    afficher_livre(L);
                } else {
                    printf("Livre non trouvé.\n");
                }
            } else {
                printf("Mauvais format.\n");
            }
            break;


        case 5 :
            printf("\nVeuillez écrire votre titre d'ouvrage : "); 
            if (scanf("%s", titre) == 1){
                Livre* L = recherche_titre(b, titre);

                if (L != NULL){
                    printf("\t=> ");
                    afficher_livre(L);
                } else {
                    printf("Livre non trouvé.\n");
                }
            } else {
                printf("Mauvais format.\n");
            }
            break;


        case 6 :
            printf("\nVeuillez écrire le nom de l'auteur de l'ouvrage.\n   => ");
            if (scanf("%s", auteur) == 1){
                Biblio* newB = recherche_auteur(b, auteur);

                if (newB->L != NULL){
                    printf("\nAffichage :\n");
                    afficher_biblio(newB);
                } else {
                    printf("Livre(s) non trouvé(s).\n");
                }

                free(newB);
            } else {
                printf("Mauvais format.\n");
            }
            break;

        
        case 7 : ;
            Biblio* test = recherche_doublon(b);

            if(test->L != NULL){
                printf("Affichage des doublons :\n");
                afficher_biblio(test);
            } else {
                printf("Aucun doublon trouvé.\n");
            }

            liberer_biblio(test);
            break;

        default:
            if (rep != 0)
                printf("Il faut un chiffre entre 0 et 7 compris ! Veuillez réessayer.\n");
            break;
        }
    } while(rep != 0);
    printf("\nMerci, et au revoir !\n\n");

    liberer_biblio(b);
    return 0;
}