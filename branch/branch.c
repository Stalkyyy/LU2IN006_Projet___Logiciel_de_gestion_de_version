#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../sha256/sha256.h"
#include "../cellList/cellList.h"
#include "../work_FileTree/work_FileTree.h"
#include "../commit/commit.h"
#include "branch.h"


/*
 * Function: initBranch
 * =======================
 * Cree le fichier cache .current_branch contenant la chaine de caracteres "master".
 * 
 * returns: void.
 */

void initBranch(){
    FILE *f = fopen(".current_branch", "w");
    if(f == NULL){
        printf("Erreur : ouverture de .current_branch (initBranch())\n");
        exit(1);
    }

    fputs("master", f);
    fclose(f);
}



/*
 * Function: branchExists
 * =========================
 * Verifie l'existence de la branche (branch).
 * 
 * returns: 1 si la branche existe.
 *          0 sinon.
 */

int branchExists(char *branch){
    char buff[1000];
    if(getcwd(buff, 1000) == NULL){
        printf("Erreur : Get Current Working Directory Path (myGit refs-list)\n");
        exit(1);
    }

    char *path = (char*)malloc(sizeof(char)*1007);
    snprintf(path, 1007, "%s/.refs", buff);

    List *l = listdir(path);
    
    free(path);

    Cell *c = searchList(l, branch);
    int res = c ? 1 : 0;

    freeList(l);
    return res;
}



/*
 * Function: createBranch
 * =========================
 * Cree une reference appelee (branch), qui pointe vers le meme commit que la reference HEAD.
 * 
 * returns: void.
 */

void createBranch(char *branch){
    char *hash = getRef("HEAD");
    createUpdateRef(branch, hash);

    if (hash != NULL) free(hash);
}



/*
 * Function: getCurrentBranch
 * =============================
 * Lit le fichier cache .current_branch pour retourner le nom de la branche courante.
 * 
 * returns: la chaine de caracteres contenant le nom de la branche courante.
 */

char* getCurrentBranch(){
    FILE *f = fopen(".current_branch", "r");
    if(f == NULL){
        printf("Erreur : ouverture de .current_branch (getCurrentBranch())\n");
        exit(1);
    }

    char *buff = (char *)malloc(sizeof(char)*256);
    if (buff == NULL){
        printf("Erreur : allocation buff (getCurrentBranch())\n");
        exit(1);
    }

    fscanf(f, "%s", buff);
    fclose(f);
    return buff;
}



/*
 * Function: printBranch
 * ========================
 * Parcourt la branche (branch), et pour chacun de ses commits, affiche son hash et son message descriptif (s'il en existe un).
 * 
 * returns: void.
 */

void printBranch(char *branch){
    char *c_hash = getRef(branch);
    if(c_hash == NULL){
        printf("Aucun commit sur %s\n", branch);
        return;
    }

    char *path_hash = hashToPath(c_hash);

    int size = strlen(path_hash) + 13;
    char *buff = (char *)malloc(sizeof(char) * size);

    snprintf(buff, size, ".autosave/%s.c", path_hash);

    Commit *c = ftc(buff);

    while(c != NULL){
        char *message = commitGet(c, "message");
        if(message != NULL){
            printf("%s %s\n", c_hash, message);
            free(message);
        }
        else{
            printf("%s\n", c_hash);
        }

        free(c_hash);
        c_hash = commitGet(c, "predecessor");

        freeCommit(c);
        if(c_hash != NULL){
            free(path_hash);
            path_hash = hashToPath(c_hash);
            snprintf(buff, size, ".autosave/%s.c", path_hash);
            c = ftc(buff);
        } else {
            c = NULL;
        }
    }

    free(c_hash);
    free(path_hash);
    free(buff);
}



/*
 * Function: branchList
 * =======================
 * Construit et retourne une liste chainee contenant le hash de tous les commits de la branche (branch).
 * 
 * note: On parcourt une branche à l'aide de la cle "predecessor".
 * 
 * returns: la liste chainee contenant les hash (seulement branch).
 */

List *branchList(char *branch){
    List *l = initList();

    char *c_hash = getRef(branch);
    char *path_hash = hashToPath(c_hash);

    int size = strlen(path_hash) + 13;
    char *buff = (char *)malloc(sizeof(char) * size);

    snprintf(buff, size, ".autosave/%s.c", path_hash);

    Commit *c = ftc(buff);
    while(c != NULL){
        insertFirst(l, buildCell(c_hash));
        free(c_hash);
        c_hash = commitGet(c, "predecessor");

        freeCommit(c);
        if(c_hash != NULL){
            free(path_hash);
            path_hash = hashToPath(c_hash);
            snprintf(buff, size, ".autosave/%s.c", path_hash);
            c = ftc(buff);
        } else {
            c = NULL;
        }
    }


    free(path_hash);
    free(buff);

    return l;
}



/*
 * Function: getAllCommits
 * ==========================
 * Renvoie la liste des hash des commits de TOUTES les branches (sans répétition).
 * 
 * returns: la liste chainee des hash (toutes les branches)
 */

List *getAllCommits(){
    List *l = initList();

    char buff[1000];
    if(getcwd(buff, 1000) == NULL){
        printf("Erreur : Get Current Working Directory Path (myGit refs-list)\n");
        exit(1);
    }

    char *path = (char*)malloc(sizeof(char)*1007);
    if(path == NULL){
        printf("Erreur : allocation path (getAllCommits())\n");
        exit(1);
    }
    snprintf(path, 1007, "%s/.refs", buff);

    List *content = listdir(path);
    Cell *c = *content;

    while(c != NULL){
        if(c->data[0] == '.')
            continue;

        List *list = branchList(c->data);
        Cell *cell = *list;
        while(cell != NULL){
            if(searchList(l, cell->data) == NULL)
                insertFirst(l, buildCell(cell->data));
            cell = cell->next;
        }

        freeList(list);
        c = c->next;
    }

    freeList(content); free(path);
    return l;
}



//============================================================================



/*
 * Function: restoreCommit
 * ==========================
 * Restaure le WorkTree associe à un commit dont le hash (hash_commit) est donne en parametre.
 * 
 * note: Il faut utiliser la fonction restoreWorkTree.
 * 
 * returns: void.
 */

void restoreCommit(char *hash_commit){
    char *path_hash = hashToPath(hash_commit);

    int size = strlen(path_hash) + 13;
    char *buff = (char *)malloc(sizeof(char) * size);
    if(buff == NULL){
        printf("Erreur : allocation buff (restoreCommit)\n");
        exit(1);
    }

    snprintf(buff, size, ".autosave/%s.c", path_hash);

    Commit *c = ftc(buff);


    char *tree = commitGet(c, "tree");
    char *tree_hash = hashToPath(tree);

    size = strlen(tree_hash) + 13;
    char *tree_file = (char *)malloc(sizeof(char) * size);
    if(tree_file == NULL){
        printf("Erreur : allocation tree_hash (restoreCommit)\n");
        exit(1);
    }

    snprintf(tree_file, size, ".autosave/%s.t", tree_hash);

    WorkTree *wt = ftwt(tree_file);
    restoreWorkTree(wt, "./");

    free(path_hash); free(buff); free(tree); free(tree_hash); free(tree_file);
    freeCommit(c);
    freeWorkTree(wt);
}



/*
 * Function: myGitCheckoutBranch
 * ================================
 * Permet de naviguer entre les branches, plus particulièrement pour aller dans la branche (branch).
 *      - On modifie le fichier .current_branch pour contenir le nom de la branche (branch).
 *      - On modifie la rererence HEAD pour contenir le hash du dernier commit de branch.
 *      - On restaure le WorkTree correspondant au dernier commit de branch.
 * 
 * returns: void.
 */

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
    if(c_hash != NULL){
        restoreCommit(c_hash);
        free(c_hash);
    }
}



/*
 * Function: filterList
 * =======================
 * Retourne une nouvelle liste contenant uniquement les elements de la liste (l) qui commencent par la chaine de caracteres (pattern).
 * 
 * returns: une liste d'elements, filtres par pattern, de la liste l.
 */

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



/*
 * Function: myGitCheckoutCommit
 * ================================
 * Permet de retourner sur n'importe quelle version du projet grace aux premiers caracteres du hash d'un commit (pattern).
 *      - On recupere la liste de tous les commits existants.
 *      - On filtre cette liste pour ne garder que ceux qui commencent par pattern.
 *      - S'il ne reste plus qu'un seul hash après ce filtre, on met a jour la reference HEAD pour pointer sur ce hash, et on restaure le WorkTree correspondant.
 *      - S'il reste plusieurs hash après ce filtre, on les affiche tous et on demande a l'utilisateur de preciser sa requete.
 *      - Sinon, s'il en reste aucun, on affiche un message d'erreur.
 * 
 * returns: void.
 */

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
            printf("Plusieurs correspondances trouvées, précisez votre requête :\n");
            while(c != NULL){
                printf("   -> %s\n", c->data);
                c = c->next;
            }
        }
    }

    freeList(l); freeList(list_filter);
}