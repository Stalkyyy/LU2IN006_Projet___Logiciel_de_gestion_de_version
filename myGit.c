#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./sha256/sha256.h"
#include "./cellList/cellList.h"
#include "./work_FileTree/work_FileTree.h"
#include "./commit/commit.h"
#include "./branch/branch.h"
#include "./fusion/fusion.h"


/*
 * Function: helpMenuPrint
 * ==========================
 * Affiche les differentes commandes possibles de myGit.
 * 
 * note: Fonction utilise si on a une mauvaise utilisation de myGit. 
 *       Plus particulierement si le deuxieme argument (init, list-refs)... est mauvais et/ou n'existe pas.
 * 
 * returns: void.
 */

void helpMenuPrint(char *nameProg){
    printf("Usage de %s :\n", nameProg);
    printf("   -> %s init\n", nameProg);
    printf("   -> %s list-refs\n", nameProg);
    printf("   -> %s create-ref <name> <hash>\n", nameProg);
    printf("   -> %s delete_ref <name>\n", nameProg);
    printf("   -> %s add <elem> [<elem2> <elem3> ...]\n", nameProg);
    printf("   -> %s list-add\n", nameProg);
    printf("   -> %s clear-add\n", nameProg);
    printf("   -> %s commit <branch_name> [-m <message>]\n", nameProg);
    printf("   -> %s get-current-branch\n", nameProg);
    printf("   -> %s branch <branch_name>\n", nameProg);
    printf("   -> %s branch-print <branch_name>\n", nameProg);
    printf("   -> %s checkout-branch <branch_name>\n", nameProg);
    printf("   -> %s checkout-commit <pattern>\n", nameProg);
    printf("   -> %s merge <branch> <message>\n", nameProg);
    printf("Pour plus d'informations, lisez le fichier README.\n");
}



/*
 * Programme : myGit
 * ====================
 * Logiciel de gestion de versions.
 * Permet le stockage, le suivi et la gestion de plusieurs versions d'un projet (ou d'un ensemble de fichiers).
 * 
 * returns: 0 si tout se passe bien.
 *          1 sinon.
 */

int main(int argc, char* argv[]){
    if(argc <= 1){
        helpMenuPrint(argv[0]);
        return 0;
    }



    /*
     * Commande : myGit init
     * ========================
     * Initialise le repertoire de references et la branche courante.
     */

    if(strcmp(argv[1], "init") == 0){
        initRefs();
        initBranch();
    }



    /*
     * Commande : myGit list-refs
     * =============================
     * Affiche toutes les references existantes.
     */

    else if(strcmp(argv[1], "list-refs") == 0){
        printf("Refs :\n");
        if(file_exists(".refs")){
            char buff[1000];
            if(getcwd(buff, 1000) == NULL){
                printf("Erreur : Get Current Working Directory Path (myGit refs-list)\n");

                return 1;
            }

            char *path = (char*)malloc(sizeof(char)*1007);
            if(path == NULL){
                printf("Erreur : allocation de path (%s refs-list)", argv[0]);
                exit(1);
            }
            snprintf(path, 1007, "%s/.refs", buff);

            List *l = listdir(path);
            Cell *c = *l;

            while(c != NULL){
                if(c->data[0] == '.')
                    continue;
                char *content = getRef(c->data);
                printf("  -> %s\t%s\n", c->data, content ? content : "");

                free(content);
                c = c->next;
            }

            freeList(l);
            free(path);
        }
    }



    /*
     * Commande : myGit create-ref <name> <hash>
     * ============================================
     * Cree la reference (name) qui pointe vers le commit correspondant à (hash).
     */

    else if(strcmp(argv[1], "create-ref") == 0){
        if (argc < 4){
            printf("Il manque des arguments.\n");
            printf("Usage : %s <name> <hash>\n", argv[0]);
            return 1;
        }

        createUpdateRef(argv[2], argv[3]);
    }



    /*
     * Commande : myGit delete-ref <name>
     * =====================================
     * Supprime la reference (name)
     * 
     * note: L'utilisateur doit faire attention s'il supprime sa current-branch.
     *       Il doit penser a faire un checkout-branch avant tout autre modification.
     */

    else if(strcmp(argv[1], "delete-ref") == 0){
        if (argc < 3){
            printf("Il manque un argument.\n");
            printf("Usae : %s <name>\n", argv[0]);
            return 1;
        }

        deleteRef(argv[2]);

        printf("Si vous supprimez votre current-branch, pensez à faire un checkout-branch avant tout autre modification !\n");
    }



    /*
     * Commande : myGit add <elem> [<elem2> <elem3> ...]
     * ====================================================
     * Ajoute un ou plusieurs fichiers/repertoires (elem, elem2, elem3...) à la zone de preparation .add (pour faire partie du prochain commit).
     */

    else if(strcmp(argv[1], "add") == 0){
        if(argc < 3){
            printf("Il manque un.des argument.s.\n");
            printf("Usage : %s add <elem> [<elem2> <elem3> ...]\n", argv[0]);
            return 1;
        }

        for(int i = 2; i < argc; i++){
            myGitAdd(argv[i]);
        }
    }



    /*
     * Commande : myGit list-add
     * ============================
     * Affiche le contenu de la zone de preparation .add
     */

    else if(strcmp(argv[1], "list-add") == 0){
        printf("Zone de préparation :\n");
        if(file_exists(".add")){
            WorkTree *wt = ftwt(".add");
            char *str_wt = wtts(wt);
            printf("%s\n", str_wt);

            freeWorkTree(wt);
            free(str_wt);
        }
    }



    /*
     * Commande : myGit clear-add
     * =============================
     * Vide la zone de preparation .add
     */

    else if(strcmp(argv[1], "clear-add") == 0){
        if(file_exists(".add"))
            system("rm .add");
    }



    /*
     * Commande : myGit commit <branch_name> [-m <message>]
     * =======================================================
     * Effectue un commit sur une branche, avec ou sans message descriptif.
     */

    else if(strcmp(argv[1], "commit") == 0){
        if (argc < 3){
            printf("Il manque un.des argument.s.\n");
            printf("Usage : %s commit <branch_name> [-m <message>]\n", argv[0]);
            return 1;
        }

        if(file_exists(".add")){
            char *message = ((argc >= 5) && (strcmp(argv[3], "-m") == 0)) ? argv[4] : NULL;
            myGitCommit(argv[2], message);
        } else {
            printf("La zone de préparation est vide/n'existe pas.\n");
        }
    }



    /*
     * Commande : myGit get-current-branch
     * ======================================
     * Affiche le nom de la branche courante.
     */

    else if(strcmp(argv[1], "get-current-branch") == 0){
        char *cur_br = getCurrentBranch();
        printf("%s\n", cur_br);
        free(cur_br);
    }



    /*
     * Commande : myGit branch <branch-name>
     * ========================================
     * Cree une branche qui s'appelle (branch-name) si elle n'existe pas deja.
     * Si elle existe deja, la commande doit afficher un message d'erreur.
     */

    else if(strcmp(argv[1], "branch") == 0){
        if(argc < 3){
            printf("Il manque un argument.\n");
            printf("Usage : %s branch <branch_name>\n", argv[0]);
            return 1;            
        }

        if(!branchExists(argv[2]))
            createBranch(argv[2]);
        else
            printf("La branch %s existe déjà !\n", argv[2]);
    }



    /*
     * Commande : myGit branch-print <branch-name>
     * ==============================================
     * Affiche le hash de tous les commits de (branch-name), accompagne de leur message descriptif eventuel.
     * Si la branche n'existe pas, un message d'erreur est affiche.
     */

    else if(strcmp(argv[1], "branch-print") == 0){
        if(argc < 3){
            printf("Il manque un argument.\n");
            printf("Usage : %s branch-print <branch_name>\n", argv[0]);
            return 1;            
        }

        if(branchExists(argv[2]))
            printBranch(argv[2]);
        else
            printf("La branch %s n'existe pas !\n", argv[2]);
    }



    /*
     * Commande : myGit checkout-branch <branch-name>
     * =================================================
     * Realise un deplacement sur la branche (branch-name).
     * Si la branche n'existe pas, un message d'erreur est affiche.
     */

    else if(strcmp(argv[1], "checkout-branch") == 0){
        if(argc < 3){
            printf("Il manque un argument.\n");
            printf("Usage : %s checkout-branch <branch_name>\n", argv[0]);
            return 1;            
        }

        if(!branchExists(argv[2])){
            printf("La branche %s n'existe pas !\n", argv[2]);
            return 1;
        }

        myGitCheckoutBranch(argv[2]);
    }



    /*
     * Commande : myGit checkout-commit <pattern>
     * =============================================
     * Realise un deplacement sur le commit qui commence par (pattern).
     * Des messages d'erreur sont affiches quand (pattern) ne correspond pas a un seul commit.
     */

    else if(strcmp(argv[1], "checkout-commit") == 0){
        if(argc < 3){
            printf("Il manque un argument.\n");
            printf("Usage : %s checkout-commit <pattern>\n", argv[0]);
            return 1;            
        }

        myGitCheckoutCommit(argv[2]);
    }



    /*
     * Commande : myGit merge <branch> <message>
     * ============================================
     * S'il n'y a pas de collision entre la branche courante et (branch), on realise le merge et on affiche un message pour montrer que la fusion s'est bien passee.
     * S'il y a des collisions, l'utilisateur doit choisir une de ces options :
     *      - Garder les fichiers de la branche courante.
     *          == On cree un commit de suppression pour (branch) et on appelle la fonction merge.
     *      - Garder les fichiers de la branche (branch).
     *          == On cree un commit de suppression pour la branche courante et on appelle la fonction merge.
     *      - Choisir manuellement, conflit par conflit, la branche sur laquelle il souhaite garder le fichier/repertoire qui est en conflit.
     *          == Il faut diviser la liste des conflits en deux listes, selon ce que l'utilisateur nous dira. Puis on cree un commit de suppression sur les deux branches grace au deux listes, avant d'utiliser la fonction merge.
     */

    else if(strcmp(argv[1], "merge") == 0){
        if(argc < 4){
            printf("Il manque un.des arguments.\n");
            printf("Usage : %s merge <branch> <message>\n", argv[0]);
            return 1;            
        }

        int size = strlen(argv[2]) + 7;
        char *br = (char *)malloc(sizeof(char) * (strlen(argv[2]) + 7));
        if(br == NULL){
            printf("Erreur : allocation de br (main - merge)\n");
            return 1;
        }

        snprintf(br, size, ".refs/%s", argv[2]);
        if(!file_exists(br)){
            printf("La branche %s n'existe pas, ré-essayez.\n", argv[2]);
            free(br);
            return 1;
        }
        free(br);

        List *conflicts = merge(argv[2], argv[3]);

        if (conflicts == NULL){
            printf("Merge effectué.\n");
            return 0;
        }

        char *curr;
        int choice;
        printf("Conflits détectés. Choisissez en tapant le chiffre correspondant :\n");
        printf("   -> 1 : Garder les fichiers de la branche courante.\n");
        printf("   -> 2 : Garder les fichiers de la branche %s.\n", argv[2]);
        printf("   -> 3 : Sélectionner un par un les fichiers et répertoires.\n");
        printf("   -> autre : Quitter.\n\n");
        printf("Mon choix : "); scanf("%d", &choice);

        switch(choice){
            case 1 :
                createDeletionCommit(argv[2], conflicts, "Suppression des conflits.");
                merge(argv[2], argv[3]);
                break;

            case 2 :
                curr = getCurrentBranch();
                myGitCheckoutBranch(argv[2]);
                createDeletionCommit(curr, conflicts, "Suppression des conflits.");
                merge(curr, argv[3]);

                free(curr);
                break;

            case 3 :
                curr = getCurrentBranch();
                List *l_current = initList();
                List *l_remote = initList();
                int choice_conflicts = 0;

                Cell *c = *conflicts;

                printf("\nChoisissez, pour chaque conflit, quel fichier vous souhaitez garder.\n");
                printf("Tapez 1 pour la branche %s (current), sinon 2 pour la branche %s.\n", curr, argv[2]);
                while(c != NULL){
                    while(choice_conflicts != 1 && choice_conflicts != 2){
                        printf("   -> conflit de \"%s\" : ", c->data);
                        scanf("%d", &choice_conflicts);
                    }

                    if(choice_conflicts == 1)
                        insertFirst(l_remote, buildCell(c->data));
                    else
                        insertFirst(l_current, buildCell(c->data));

                    c = c->next;
                    choice_conflicts = 0;
                }

                createDeletionCommit(curr, l_current, "Suppression des conflits.");
                createDeletionCommit(argv[2], l_remote, "Suppression des conflits.");
                merge(argv[2], argv[3]);

                printf("\nMerge effectué.\n");

                freeList(l_current); freeList(l_remote); free(curr);
                break;

            default :
                printf("Au-revoir !\n");
                break;
        }

        freeList(conflicts);
        return 0;
    }



    else helpMenuPrint(argv[0]);

    return 0;
}