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

kvp* createKeyVal(char *key, char *val){
    kvp* kv = (kvp*)(malloc(sizeof(kvp)));
    if(kv == NULL){
        printf("Erreur : allocation key_val_pair (createKeyVal)\n");
        exit(1);
    }

    kv->key = key ? strdup(key) : NULL;
    kv->value = val ? strdup(val) : NULL;

    return kv;
}

void freeKeyVal(kvp *kv){
    free(kv->key);
    free(kv->value);
    free(kv);
}


char* kvts(kvp *k){
    char *str = (char*)malloc(sizeof(char) * (strlen(k->key) + strlen(k->value) + 2));
    strcpy(str, k->key);
    strcat(str, ":");
    strcat(str, k->value);

    return str;
}

kvp* stkv(char *str){
    int pos = -1;
    for(int i = 0; i < strlen(str); i++){
        if (str[i] == ':'){
            pos = i;
            break;
        }
    }

    if (pos == -1){
        printf("Ce n'est pas une chaine de caractères de la forme \"clé:valeur\"\n");
        return NULL;
    }

    char *key = (char*)malloc(sizeof(char) * pos + 1);
    if (key == NULL){
        printf("Erreur : allocation key (stkv)\n");
        exit(1);
    }

    strncpy(key, str, pos);
    key[pos] = '\0';
    kvp* kv = createKeyVal(key, str + pos + 1);
    
    free(key);
    return kv;
}



//=====================================================================



Commit* initCommit(){
    Commit *c = (Commit*)malloc(sizeof(Commit));
    if(c == NULL){
        printf("Erreur : allocation commit (initCommit)\n");
        exit(1);
    }

    c->T = (kvp**)malloc(sizeof(kvp*) * MAX_SIZE_COMMIT);
    if(c->T == NULL){
        printf("Erreur : allocation commit->T (initCommit)\n");
        exit(1);
    }

    c->n = 0;
    c->size = MAX_SIZE_COMMIT;

    for(int i = 0; i < c->size; i++)
        c->T[i] = NULL;

    return c;
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


/*
 * Choix de la fonction de hachage : sdbm.
*/

unsigned long hash_sdbm(char *str){
    unsigned long hash = 0;
    int c;
    while((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}


void commitSet(Commit *c, char *key, char *value){
    if(c->n >= c->size){
        printf("Le tableau est déjà rempli !\n");
        return;
    }

    kvp *kv = createKeyVal(key, value);

    unsigned long hash = hash_sdbm(key) % c->size;

    for(int i = 0; i < c->size; i++){
        int index = (hash + i) % c->size;
        if(c->T[index] == NULL){
            c->T[index] = kv;
            c->n++;
            return;
        }
    }
}

/*
 * Rajout de cette fonction. Même fonctionnement que commitSet mais prend directement un kvp en paramètre.
 * Fonction utilisée pour stc.
*/
void commitSet_KV(Commit *c, kvp *kv){
    if(c->n >= c->size){
        printf("Le tableau est déjà rempli !\n");
        return;
    }

    unsigned long hash = hash_sdbm(kv->key) % c->size;

    for(int i = 0; i < c->size; i++){
        int index = (hash + i) % c->size;
        if(c->T[index] == NULL){
            c->T[index] = kv;
            c->n++;
            return;
        }
    }
}


Commit* createCommit(char *hash){
    Commit *c = initCommit();
    commitSet(c, "tree", hash);
    return c;
}


char* commitGet(Commit *c, char *key){
    unsigned long hash = hash_sdbm(key) % c->size;

    for(int i = 0; i < c->size; i++){
        int index = (hash + i) % c->size;

        if(c->T[index] == NULL) 
            return NULL;
        
        if(strcmp(c->T[index]->key, key) == 0)
            return c->T[index]->key;
    }

    return NULL;
}


int tailleCommitHash_STR(Commit *c){
    int taille = 0;
    int nbKvp = c->n;
    for(int i = 0; (i < c->size) && (nbKvp > 0); i++){
        if(c->T[i] != NULL){
            taille += strlen(c->T[i]->key) + 1 + strlen(c->T[i]->value) + 1;
            nbKvp--;
        }
    }

    return taille;
}

char* cts(Commit *c){
    char *str = (char*)malloc(sizeof(char) * (tailleCommitHash_STR(c) + 1));
    strcpy(str, "");

    int nbKvp = c->n;
    for(int i = 0; (i < c->size) && (nbKvp > 0); i++){
        if(c->T[i] != NULL){
            strcat(str, c->T[i]->key);
            strcat(str, ":");
            strcat(str, c->T[i]->value);
            strcat(str, "\n");
            nbKvp--;
        }
    }

    return str;
}

Commit* stc(char *ch){
    Commit *c = initCommit();
    char *tok = strtok(ch, "\n");

    while(tok != NULL){
        kvp *kv = stkv(tok);
        commitSet_KV(c, kv);
        tok = strtok(NULL, "\n");
    }

    return c;
}


void ctf(Commit *c, char *file){
    char *str = cts(c);

    FILE *f = fopen(file, "w");
    if(f == NULL){
        printf("Erreur : ouverture de %s (ctf)\n", file);
        exit(1);
    }

    fprintf(f, "%s", str);

    free(str);
    fclose(f);
}

Commit* ftc(char *file){
    Commit *c = initCommit();
    FILE *f = fopen(file, "r");
    if(f == NULL){
        printf("Erreur : ouverture de %s (ftc)\n", file);
        exit(1);
    }

    char key[512];
    char value[512];

    while((EOF != fscanf(f, "%[^:]:%[^\n]\n", key, value)) && (c->n < c->size)){
        commitSet(c, key, value);
    }

    fclose(f);
    return c;
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
     * Même fonction que BlobFile, à la différence près qu'on rajoute ".c" à la fin du nom de l'instantané.
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