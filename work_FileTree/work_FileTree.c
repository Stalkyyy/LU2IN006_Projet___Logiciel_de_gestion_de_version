#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "work_FileTree.h"
#include "../sha256/sha256.h"
#include "../cellList/cellList.h"

int getChmod(const char *path){
    struct stat ret;
    if(stat(path, &ret) == -1)
        return -1;

    return ( ret . st_mode & S_IRUSR ) |( ret . st_mode & S_IWUSR ) |( ret . st_mode & S_IXUSR ) | ( ret . st_mode & S_IRGRP ) |( ret . st_mode & S_IWGRP ) |( ret . st_mode & S_IXGRP ) | ( ret . st_mode & S_IROTH ) |( ret . st_mode & S_IWOTH ) |( ret . st_mode & S_IXOTH ) ;
}

void setMode(int mode, char *path){
    char buff[100];
    sprintf(buff, "chmod %d %s", mode, path);
    system(buff);
}


//===============================================================================


WorkFile* createWorkFile(char *name){
    WorkFile *wf = (WorkFile *)malloc(sizeof(WorkFile));
    if (wf == NULL){
        printf("Erreur : allocation WorkFile\n");
        exit(1);
    }

    wf->name = strdup(name);
    wf->hash = NULL;
    wf->mode = 0;

    return wf;
}


void freeWorkFile(WorkFile *wf){
    free(wf->name);
    free(wf->hash);
    free(wf);
}


char* wfts(WorkFile *wf){
    int length = strlen(wf->name) + strlen(wf->hash) + 6;
    char *str = (char *)malloc(sizeof(char)*length);

    char *mode = (char *)malloc(sizeof(char)*3);
    sprintf(mode, "%d", wf->mode);
    
    strcat(str, wf->name);
    strcat(str, "\t");
    strcat(str, wf->hash);
    strcat(str, "\t");
    strcat(str, mode);

    free(mode);
    return str;
}


WorkFile* stwf(char *ch){
    char *ptr = strtok(ch, "\t");
    WorkFile *wf = createWorkFile(ptr);

    ptr = strtok(NULL, "\t");
    wf->hash = strdup(ptr);
    ptr = strtok(NULL, "\t");
    wf->mode = atoi(ptr);

    return wf;
}


//===============================================================================


WorkTree* initWorkTree(){
    WorkTree *wt = (WorkTree *)malloc(sizeof(WorkTree));
    if (wt == NULL){
        printf("Erreur : allocation Worktree\n");
        exit(1);
    }

    wt->tab = (WorkFile *)malloc(sizeof(WorkFile)*MAX_TAB_WF);
    if (wt->tab == NULL){
        printf("Erreur : allocation tableau Worktree\n");
        exit(1);
    }

    wt->size = MAX_TAB_WF;
    wt->n = 0;

    return wt;
}

void freeWorkTree(WorkTree *wt){
    for(int i = 0; i < wt->n; i++){
        WorkFile wf = wt->tab[i];
        free(wf.name);
        free(wf.hash);
    }
        

    free(wt->tab);
    free(wt);
}


int inWorkTree(WorkTree *wt, char *name){
    for(int i = 0; i < wt->n; i++){
        if (strcmp((wt->tab[i]).name, name) == 0)
            return i;
    }

    return -1;
}


int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode){

    if( wt->n == wt->size){
        printf("Tableau de WorkFile rempli !\n");
        return 1;
    }
    if (inWorkTree(wt, name) != -1){
        printf("\"%s\" déjà présent dans le tableau !\n", name);
        return 1;
    }

    WorkFile *wf = createWorkFile(name);
    wf->hash = strdup(hash);
    wf->mode = mode;

    wt->tab[wt->n] = *wf;
    wt->n++;
    return 0;
}
