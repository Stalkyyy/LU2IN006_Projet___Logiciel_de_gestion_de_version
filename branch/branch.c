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


void initBranch(){
    FILE *f = fopen(".current_branch", "w");
    if(f == NULL){
        printf("Erreur : ouverture de .current_branch (initBranch())\n");
        exit(1);
    }

    fputs("master", f);
    fclose(f);
}


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


void createBranch(char *branch){
    char *hash = getRef("HEAD");
    createUpdateRef(branch, hash);

    if (hash != NULL) free(hash);
}


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


void printBranch(char *branch){
    char *c_hash = getRef(branch);
    char *path_hash = hashToPath(c_hash);

    char *buff = (char *)malloc(sizeof(char) * (strlen(path_hash) + 13));
    strcpy(buff, ".autosave/");
    strcat(buff, path_hash);
    strcat(buff, ".c");

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
            strcpy(buff, ".autosave/");
            strcat(buff, path_hash);
            strcat(buff, ".c");

            c = ftc(buff);
        } else {
            c = NULL;
        }
    }

    free(c_hash);
    free(path_hash);
    free(buff);
}


List *branchList(char *branch){
    List *l = initList();

    char *c_hash = getRef(branch);
    char *path_hash = hashToPath(c_hash);

    char *buff = (char *)malloc(sizeof(char) * (strlen(path_hash) + 13));
    strcpy(buff, ".autosave/");
    strcat(buff, path_hash);
    strcat(buff, ".c");

    Commit *c = ftc(buff);
    while(c != NULL){
        insertFirst(l, buildCell(c_hash));
        free(c_hash);
        c_hash = commitGet(c, "predecessor");

        freeCommit(c);
        if(c_hash != NULL){
            free(path_hash);
            path_hash = hashToPath(c_hash);
            strcpy(buff, ".autosave/");
            strcat(buff, path_hash);
            strcat(buff, ".c");

            c = ftc(buff);
        } else {
            c = NULL;
        }
    }


    free(path_hash);
    free(buff);

    return l;
}


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