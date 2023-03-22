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
    if (wf->hash != NULL)
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
 * Nous avons préféré faire un tableau de POINTEURS de Workfile.
 * En effet, nous avions déjà fait une fonction freeWorkFile.
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
    wf->hash = hash ? strdup(hash) : NULL;
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
     * Cette boucle permet de stocker chaque chaine de caractère des workFile, ainsi qu'avoir la taille du string sortant.
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


//===============================================================================


char* blobWorkTree(WorkTree *wt){
    /*
     * Création du fichier temporaire afin d'avoir la hash du WorkTree.
    */
    if(!file_exists("tmp")){
        system("mkdir tmp");
    }

    static char template[] = "./tmp/myfileXXXXXX";
    char fname[1000];
    strcpy(fname, template);
    int fd = mkstemp(fname);
    wttf(wt, fname);
    close(fd);

    
    /*
     * Même fonction que BlobFile, à la différence près qu'on rajoute ".t" à la fin du nom de l'instantané.
    */
    char *hash = sha256file(fname);
    char rep[12];
    snprintf(rep, 13, "autosave/%c%c", hash[0], hash[1]);

    char linuxCommand[1022];
    if(!file_exists("autosave")){
        system("mkdir autosave");
    }
    if(!file_exists(rep)){
        snprintf(linuxCommand, 21, "mkdir -p autosave/%c%c", hash[0], hash[1]);
        system(linuxCommand);
    }

    char *path = hashToPath(hash);
    char *hashFileName = malloc(sizeof(char)*(strlen(path)+12));
    if (hashFileName == NULL){
        printf("Erreur : allocation de HashFileName (BlobWorkTree)\n");
        exit(1);
    }

    strcpy(hashFileName, "autosave/");
    strcat(hashFileName, path);
    strcat(hashFileName, ".t");
    cp(hashFileName,fname);
    free(hashFileName);
    free(path);


    /*
     * On supprime le fichier temporaire.
    */
    snprintf(linuxCommand, 1022, "rm -f %s", fname+2);
    system(linuxCommand);

    return hash;
}


char* saveWorkTree(WorkTree *wt, char *path){
    struct stat ret;

    for(int i = 0; i < wt->n; i++){
        char *pathFile = (char*)malloc(sizeof(char)*(strlen(path) + strlen(wt->tab[i]->name) + 2));
        if(pathFile == NULL){
            printf("Erreur : allocation de pathFile (saveWorkTree)\n");
            exit(1);
        }
        strcpy(pathFile, path);
        strcat(pathFile, wt->tab[i]->name);

        if(stat(pathFile, &ret) != 1){
            if(S_ISREG(ret.st_mode)){
                blobFile(pathFile);
                wt->tab[i]->hash = sha256file(pathFile);
                wt->tab[i]->mode = getChmod(pathFile);
            }

            else if (S_ISDIR(ret.st_mode)){
                char rootPathFile[1000];
                getcwd(rootPathFile,1000);
                strcat(rootPathFile, "/");
                strcat(rootPathFile, pathFile);

                WorkTree *newWT = initWorkTree();
                List *l = listdir(rootPathFile);
                Cell *c = *l;
                while(c){
                    appendWorkTree(newWT, c->data, NULL, 0);
                    c = c->next;
                }

                strcat(pathFile, "/");

                wt->tab[i]->hash = saveWorkTree(newWT, pathFile);
                wt->tab[i]->mode = getChmod(pathFile);
                freeWorkTree(newWT);
                freeList(l);
            }
        }

        free(pathFile);
    }

    return blobWorkTree(wt);
}


/*
 * Ces variables serviront à enregistrer la position du dossier autosave.
 * Nous supposons que, lors de l'appel de la fonction, le path est le répertoire parent du autosave.
 * On a qu'un seul autosave par projet.
*/
int testOldPath_Autosave = 0;
char *OldPath_Autosave = NULL;

void restoreWorkTree(WorkTree *wt, char *path){
    for(int i = 0; i < wt->n; i++){
        char *hashPath = hashToPath(wt->tab[i]->hash);

        /*
         * Ici, 12 signifie : 1 pour '\0', 2 pour '.t' si besoin, 9 pour "autosave/".
        */
        char *pathFile = (char*)malloc(sizeof(char) * (strlen(path) + strlen(hashPath) + 12));
        if(pathFile == NULL){
                printf("Erreur : allocation de pathFile (restoreWorkTree)\n");
                exit(1);
            }

        /*
         * Si c'est la première fois qu'on tombe sur un path, on l'enregistre. Il contient le dossier autosave.
         * Sinon, durant le strcpy, on prend le premier path contenu dans la variable OldPath_Autosave.
        */
        if(testOldPath_Autosave == 0){
            strcpy(pathFile, path);
            testOldPath_Autosave = 1;
            OldPath_Autosave = path;
        } else {
            strcpy(pathFile, OldPath_Autosave);
        }
        strcat(pathFile, "autosave/");
        strcat(pathFile, hashPath);

        if(file_exists(pathFile)){
            char *pathCopie = (char*)malloc(sizeof(char) * (strlen(path) + strlen(wt->tab[i]->name) + 1));
            if(pathCopie == NULL){
                printf("Erreur : allocation de pathCopie (restoreWorkTree)\n");
                exit(1);
            }
            strcpy(pathCopie, path);
            strcat(pathCopie, wt->tab[i]->name);

            cp(pathCopie, pathFile);

            mode_t mode = wt->tab[i]->mode;
            if(chmod(pathCopie, mode) == -1){
                printf("Erreur : changement de mode de %s\n", pathCopie);
                exit(1);
            }

            free(pathCopie);
        }
        
        else{
            strcat(pathFile, ".t");
            if(file_exists(pathFile)){
                char *newPath = (char*)malloc(sizeof(char) * (strlen(path) + strlen(wt->tab[i]->name) + 2));
                if(newPath == NULL){
                    printf("Erreur : allocation de newPath (restoreWorkTree)\n\n");
                    exit(1);
                }

                strcpy(newPath, path);
                strcat(newPath, wt->tab[i]->name);
                strcat(newPath, "/");

                if(!file_exists(newPath)){
                    char *linuxCommand = (char *)malloc(sizeof(char) * (strlen(path) + strlen(wt->tab[i]->name) + 13));
                    snprintf(linuxCommand, 9 + strlen(newPath) , "mkdir -p %s", newPath);
                    system(linuxCommand);
                    free(linuxCommand);
                }

                WorkTree *newWT = ftwt(pathFile);
                restoreWorkTree(newWT, newPath);

                mode_t mode = wt->tab[i]->mode;
                if(chmod(newPath, mode) == -1){
                    printf("Erreur : changement de mode de %s\n", newPath);
                    exit(1);
                }

                freeWorkTree(newWT);
                free(newPath);
            }

            else {
                printf("Le fichier instantané de %s est introuvable dans %s\n\n", wt->tab[i]->name, OldPath_Autosave);
            }
            
        }

        free(hashPath);
        free(pathFile);
    }
}