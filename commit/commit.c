#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "commit.h"
#include "../sha256/sha256.h"
#include "../cellList/cellList.h"
#include "../work_FileTree/work_FileTree.h"

//Exercice 6
kvp* createKeyVal(char* key,  char* val){
    kvp* new = (kvp*)malloc(sizeof(kvp));
    if(new == NULL){
        printf("Erreur d'allocation (createKeyVal)\n");
        exit(1);
    }

    new->key = strdup(key);
    new->value = strdup(val);

    return new;
}

void freeKeyVal(kvp* k){
    free(k->key);
    free(k->value);
    free(k);
}

char* kvts(kvp* k){
    char* res = malloc(sizeof(char)*(strlen(k->value) + strlen(k->key)+ 2));
    res = strcpy(res, k->key);
    res = strcat(res, ":");
    res = strcat(res, k->value);
    return res;
}

kvp* stkv(char* str){
    for(int i = 0; i<strlen(str); i++){
        if(str[i] == ':'){
            char* key = malloc(sizeof(char)* i+1);
             if (key == NULL){
                printf("Erreur d'allocation (stkv)\n");
                exit(1);
            }
            key  = strncpy(key, str, i);
            key[i] = '\0';
            char* val = malloc(sizeof(char)* (strlen(str) - i));
            if (val == NULL){
                printf("Erreur d'allocation (stkv)\n");
                exit(1);
            }
            val = strcpy(val, str+i+1);
            kvp* kv = createKeyVal(key,val);
            free(key);
            free(val);
            return kv;
        }
    }

    printf("Usage: 'key:val'");
    return NULL;
}

Commit* initCommit(){
    Commit* new = (Commit*)malloc(sizeof(Commit));
    if(new == NULL){
        printf("Erreur d'allocation (initCommit)\n");
        exit(1);
    }

    new->T =(kvp**)malloc(sizeof(kvp)*TAILLE_COMMIT);
    if(new->T == NULL){
        printf("Erreur d'allocation (initCommit)\n");
        exit(1);
    }

    new->n = 0;
    new->size = TAILLE_COMMIT;

    for(int i = 0; i<new->size;i++){
        new->T[i] = NULL;
    }
    return new;

}

void freeCommit(Commit *c){
    for(int i = 0; (i < c->size) && (c->n > 0); i++){
        if(c->T[i] != NULL){
            freeKeyVal(c->T[i]);
            c->n--;
        }
    }

    free(c->T);
    free(c);
}


void affichageCommit(Commit *c){
    printf("Commit de taille %d avec %d KVPs\n", c->size, c->n);
    for(int i = 0; i < c->size; i++){
        if(c->T[i] != NULL)
            printf("%s => %s\n", c->T[i]->key, c->T[i]->value);
    }
    printf("\n");
}

//fonction de hachage: sdbm

unsigned long hash_sdbm(char *str){
    unsigned long hash = 0;
    int c;
    while((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}

void commitSet(Commit* c, char* key, char* value){
    if(c->n >= c->size){
        printf("Le tableau est rempli!\n");
        return;
    }

    kvp* new = createKeyVal(key, value);

    unsigned long hash = hash_sdbm(key) % c->size;

    //probing lineaire
    int i;
    for(i = 0; i < c->size; i++){
        int j = (hash + i) % c->size;
        if(c->T[j] == NULL){
            c->T[j] = new;
            c->n++;
            return;
        }
    }

}

Commit* createCommit(char* hash){
    Commit* new = initCommit();
    commitSet(new, "tree", hash);
    return new;
}


char* commitGet(Commit* c, char* key){
    unsigned long int hash = hash_sdbm(key) % c->size;

    int i;
    for(i = 0; i< c->size; i++){
        int j = (hash + i) % c->size;

        if(c->T[j] == NULL){
            return NULL;
        }   

        if(strcmp(c->T[j]->key, key) == 0){
            return c->T[j]->value;
        }
    }

    return NULL;
}

int lenstrCommit(Commit* c){
    int i = 0, j = 0, res = 0;
    while(i<c->size && j < c->n){
        if(c->T[i] != NULL){
            res+=strlen(c->T[i]->key) + strlen(c->T[i]->value) + 2;
            j++;
        }
        i++;
    }
    return res;
}

char* cts(Commit* c){
    char* str = malloc(sizeof(char)*lenstrCommit(c)+100);
    int i = 0, j = 0;
    strcpy(str, "");
    while(i<c->size && j < c->n){
        if(c->T[i] != NULL){
            str = strcat(str, c->T[i]->key);
            str = strcat(str, ":");
            str = strcat(str, c->T[i]->value);
            str = strcat(str, "\n");

            j++;
        }
        i++;
    }

    return str;
}

Commit* stc(char* ch){
    Commit* new = initCommit();
    char* strToken = strtok(ch, "\n");

    while(strToken != NULL){
        kvp* kv = stkv(strToken);
        commitSet(new, kv->key, kv->value);
        freeKeyVal(kv);
        strToken = strtok(NULL, "\n");
    }

    return new;
}
void ctf(Commit* c, char* file){
    char* str = cts(c);
    FILE* f = fopen(file, "w");
    if(f == NULL){
        printf("Erreur d'ouverture de fichier (ctf)");
        exit(1);
    }

    fprintf(f, "%s", str);
    free(str);
    fclose(f);
}

Commit* ftc(char* file){
    Commit* new = initCommit();
    FILE *f = fopen(file, "r");
    if(f == NULL){
        printf("Erreur d'ouverture de fichier (ftc)");
        exit(1);
    }
    char key[200];char value[200];

    while(fscanf(f, "%[^:]:%[^\n]\n@", key, value) != EOF){
        commitSet(new,key,value);
    }

    fclose(f);
    return new;
}

char* blobCommit(Commit *c){

        /*
     * Création du fichier temporaire afin d'avoir la hash du WorkTree.
    */
    if(!file_exists("tmp")){
        system("mkdir tmp");
    }

    static char template[] = "./tmp/blobCommit_XXXXXX";
    char fname[1000];
    strcpy(fname, template);
    int fd = mkstemp(fname);
    ctf(c, fname);
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
        printf("Erreur : allocation de HashFileName (BlobCommit)\n");
        exit(1);
    }

    strcpy(hashFileName, "autosave/");
    strcat(hashFileName, path);
    strcat(hashFileName, ".c");
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
