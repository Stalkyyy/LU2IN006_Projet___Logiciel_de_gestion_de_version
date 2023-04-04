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

int main(int argc, char* argv[]){
    if(argc <= 1){
        printf("Usage de %s :\n", argv[0]);
        printf("   -> %s init\n", argv[0]);
        printf("   -> %s list-refs\n", argv[0]);
        printf("   -> %s create-ref <name> <hash>\n", argv[0]);
        printf("   -> %s delete_ref <name>\n", argv[0]);
        printf("   -> %s add <elem> [<elem2> <elem3> ...]\n", argv[0]);
        printf("   -> %s list-add\n", argv[0]);
        printf("   -> %s clear-add\n", argv[0]);
        printf("   -> %s commit <branch_name> [-m <message>]\n", argv[0]);
        printf("   -> %s get-current-branch\n", argv[0]);
        printf("   -> %s branch <branch_name>\n", argv[0]);
        printf("   -> %s branch-print <branch_name>\n", argv[0]);
        printf("   -> %s checkout-branch <branch_name>\n", argv[0]);
        printf("   -> %s checkout-commit <pattern>\n", argv[0]);

        return 0;
    }

    if(strcmp(argv[1], "init") == 0){
        initRefs();
        initBranch();
    }

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

    else if(strcmp(argv[1], "create-ref") == 0){
        if (argc < 4){
            printf("Il manque des arguments.\n");
            printf("Usage : %s <name> <hash>\n", argv[0]);
            return 1;
        }

        createUpdateRef(argv[2], argv[3]);
    }

    else if(strcmp(argv[1], "delete-ref") == 0){
        if (argc < 3){
            printf("Il manque un argument.\n");
            printf("Usae : %s <name>\n", argv[0]);
            return 1;
        }

        deleteRef(argv[2]);

        printf("Si vous supprimez votre current-branch, pensez à faire un checkout-branch avant tout autre modification !\n");
    }

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

    else if(strcmp(argv[1], "clear-add") == 0){
        if(file_exists(".add"))
            system("rm .add");
    }

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

    else if(strcmp(argv[1], "get-current-branch") == 0){
        char *cur_br = getCurrentBranch();
        printf("%s\n", cur_br);
        free(cur_br);
    }

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

    else if(strcmp(argv[1], "checkout-commit") == 0){
        if(argc < 3){
            printf("Il manque un argument.\n");
            printf("Usage : %s checkout-commit <pattern>\n", argv[0]);
            return 1;            
        }

        myGitCheckoutCommit(argv[2]);
    }




    else {
        printf("Usage de %s :\n", argv[0]);
        printf("   -> %s init\n", argv[0]);
        printf("   -> %s list-refs\n", argv[0]);
        printf("   -> %s create-ref <name> <hash>\n", argv[0]);
        printf("   -> %s delete_ref <name>\n", argv[0]);
        printf("   -> %s add <elem> [<elem2> <elem3> ...]\n", argv[0]);
        printf("   -> %s list-add\n", argv[0]);
        printf("   -> %s clear-add\n", argv[0]);
        printf("   -> %s commit <branch_name> [-m <message>]\n", argv[0]);
        printf("   -> %s get-current-branch\n", argv[0]);
        printf("   -> %s branch <branch_name>\n", argv[0]);
        printf("   -> %s branch-print <branch_name>\n", argv[0]);
        printf("   -> %s checkout-branch <branch_name>\n", argv[0]);
        printf("   -> %s checkout-commit <pattern>\n", argv[0]);
    }

    return 0;
}