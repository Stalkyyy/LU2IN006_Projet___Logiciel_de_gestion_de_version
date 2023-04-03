#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./sha256/sha256.h"
#include "./cellList/cellList.h"
#include "./work_FileTree/work_FileTree.h"
#include "./commit/commit.h"
#include "./branch/branch.h"

void myGitAdd(char *file_or_folder){
    /*
     * Ici, le fichier .add sera dans tous les cas créer par la fonction wttf s'il n'existe pas.
     */
    WorkTree *wt = file_exists(".add") ? ftwt(".add") : initWorkTree();

    if(file_exists(file_or_folder)){
        appendWorkTree(wt, file_or_folder, NULL, 0);
        wttf(wt, ".add");
    } else {
        printf("Le fichier/répertoire %s n'existe pas.\n", file_or_folder);
    }

    freeWorkTree(wt);
}


void myGitCommit(char *branch_name, char *message){
    if(!file_exists(".refs")){
        printf("Initialisez d'abord les références du projet.\n");
        return;
    }

    char *buff = (char *)malloc(sizeof(char) * (strlen(branch_name) + 7));
    if(buff == NULL){
        printf("Erreur : allocation buff (myGitCommit)\n");
        exit(1);
    }
    sprintf(buff, ".refs/%s", branch_name);

    if(!file_exists(buff)){
        printf("La branche %s n'existe pas.\n", branch_name);
        free(buff);
        return;
    }

    char *hash_branch = getRef(branch_name);
    char *hash_head = getRef("HEAD");

    if(hash_head != NULL){
        if(strcmp(hash_branch, hash_head)){
                printf("HEAD doit pointer sur le dernier commit de la branche.\n");
                free(buff); free(hash_branch); free(hash_head);
                return;
        }
    }

    WorkTree *wt = ftwt(".add");
    system("rm .add");

    char *hash_wt = saveWorkTree(wt, "./");

    Commit *c = createCommit(hash_wt);

    if (hash_branch != NULL){
        if(strlen(hash_branch) != 0)
            commitSet(c, "predecessor", hash_branch);
    }

    if (message != NULL)
        commitSet(c, "message", message);

    char *hash_c = blobCommit(c);
    createUpdateRef(branch_name, hash_c);
    createUpdateRef("HEAD", hash_c);
    
    free(buff); free(hash_branch); free(hash_head); free(hash_wt); free(hash_c);
    freeWorkTree(wt);
    freeCommit(c);
}


//=====================================================================================


void restoreCommit(char *hash_commit){
    char *path_hash = hashToPath(hash_commit);

    char *buff = (char *)malloc(sizeof(char) * (strlen(path_hash) + 13));
    if(buff == NULL){
        printf("Erreur : allocation buff (restoreCommit)\n");
        exit(1);
    }

    strcpy(buff, ".autosave/");
    strcat(buff, path_hash);
    strcat(buff, ".c");

    Commit *c = ftc(buff);


    char *tree = commitGet(c, "tree");
    char *tree_hash = hashToPath(tree);
    char *tree_file = (char *)malloc(sizeof(char) * (strlen(tree_hash) + 13));
    if(buff == NULL){
        printf("Erreur : allocation tree_hash (restoreCommit)\n");
        exit(1);
    }
    strcpy(tree_file, ".autosave/");
    strcat(tree_file, tree_hash); 
    strcat(tree_file, ".t");

    WorkTree *wt = ftwt(tree_file);
    restoreWorkTree(wt, "./");

    free(path_hash); free(buff); free(tree); free(tree_hash); free(tree_file);
    freeCommit(c);
    freeWorkTree(wt);
}


void myGitCheckoutBranch(char *branch){
    FILE *f = fopen(".current_branch", "w");
    if(f == NULL){
        printf("Erreur : ouverture de .current_branch (myGitCheckoutBranch())\n");
        exit(1);
    }

    fprintf(f, "%s", branch);
    fclose(f);

    char *c_hash = getRef(branch);
    createUpdateRef("HEAD", c_hash);
    restoreCommit(c_hash);
    free(c_hash);
}


List *filterList(List *l, char *pattern){
    List *list_filter = initList();

    Cell *c = *l;
    while(c){
        char *str = strdup(c->data);
        str[strlen(pattern)] = '\0';

        if (strcmp(str, pattern) == 0){
            insertFirst(list_filter, buildCell(c->data));
        }
        
        free(str);
        c = c->next;
    }

    return list_filter;
}


void myGitCheckoutCommit(char *pattern){
    List *l = getAllCommits();
    List *list_filter = filterList(l, pattern);

    if(*list_filter == NULL){
        printf("Pas de pattern trouvé.\n");
    }

    else{
        Cell *c = *list_filter;
        if(c->next == NULL){
            char *c_hash = strdup(c->data);
            createUpdateRef("HEAD", c_hash);
            restoreCommit(c_hash);
            free(c_hash);
        }

        else {
            printf("Plusieurs correspondances trouvées :\n");
            while(c != NULL){
                printf("   -> %s\n", c->data);
                c = c->next;
            }
        }
    }

    freeList(l); freeList(list_filter);
}


//=====================================================================================



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