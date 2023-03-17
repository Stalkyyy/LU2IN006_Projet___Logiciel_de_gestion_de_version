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
    if(str == NULL){
        printf("Erreur : Allocation str wtfs()\n");
        exit(1);
    }

    char *mode = (char *)malloc(sizeof(char)*4);
    if(mode == NULL){
        printf("Erreur : Allocation mode (str) wtts()\n");
        exit(1);
    }
    sprintf(mode, "%d", wf->mode);
    
    strcpy(str, wf->name);
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


/*
    Nous avons préféré faire un tableau de POINTEURS de Workfile.
    En effet, nous avions déjà fait une fonction freeWorkFile.
*/
WorkTree* initWorkTree(){
    WorkTree *wt = (WorkTree *)malloc(sizeof(WorkTree));
    if (wt == NULL){
        printf("Erreur : allocation Worktree\n");
        exit(1);
    }

    wt->tab = (WorkFile **)malloc(sizeof(WorkFile*)*MAX_TAB_WF);
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
        freeWorkFile(wt->tab[i]);
    }
        
    free(wt->tab);
    free(wt);
}


int inWorkTree(WorkTree *wt, char *name){
    for(int i = 0; i < wt->n; i++){
        if (strcmp((wt->tab[i])->name, name) == 0)
            return i;
    }

    return -1;
}


int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode){
    if(wt->n == wt->size){
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

    wt->tab[wt->n] = wf;
    wt->n++;
    return 0;
}


char* wtts(WorkTree *wt){
    int sizeStr = 0;
    char **tab = (char**)malloc(sizeof(char *)*wt->n);
    if(tab == NULL){
        printf("Erreur : Allocation tab wtts()\n");
        exit(1);
    }

    /*
        Cette boucle permet de stocker chaque chaine de caractère des workFile, ainsi qu'avoir la taille du string sortant.
    */
    for(int i = 0; i < wt->n; i++){
        char* strWf = wfts(wt->tab[i]);
        sizeStr += strlen(strWf) + 1;
        tab[i] = strWf;
    }

    char *str = (char*)malloc(sizeof(char)*(sizeStr + 1));
    if(str == NULL){
        printf("Erreur : Allocation str wtts()\n");
        exit(1);
    }

    strcpy(str, "");

    for(int i = 0; i < wt->n; i++){
        strcat(str, tab[i]);
        strcat(str, "\n");
        free(tab[i]);
    }

    free(tab);
    return str;
}


WorkTree* stwt(char *s){
    WorkTree *wt = initWorkTree();
    char *tok = strtok(s, "\t\n");

    while((tok != NULL) && (wt->n < wt->size)){
        char *name = tok;
        char *hash = strtok(NULL, "\t\n");
        int mode = atoi(strtok(NULL, "\t\n"));

        appendWorkTree(wt, name, hash, mode);
        tok = strtok(NULL, "\t\n");
    }

    return wt;
}


int wttf(WorkTree *wt, char *file){
    char *str = wtts(wt);

    FILE *f = fopen(file, "w");
    if(f == NULL){
        printf("Erreur : ouverture de %s (wttf)\n", file);
        exit(1);
    }

    fprintf(f, "%s", str);

    free(str);
    fclose(f);
    return 0;
}


WorkTree* ftwt(char* file){
    WorkTree *wt = initWorkTree();
    FILE *f = fopen(file, "r");
    if(f == NULL){
        printf("Erreur : ouverture de %s (ftwt)\n", file);
        exit(1);
    }

    char name[256];
    char hash[256];
    int mode;

    while((EOF != fscanf(f, "%s\t%s\t%d\n", name, hash, &mode)) && (wt->n < wt->size)){
        appendWorkTree(wt, name, hash, mode);
    }

    fclose(f);
    return wt;
}