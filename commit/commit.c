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


/*
 * Function: createKeyVal
 * =========================
 * Alloue et initialise un element du type key_value_pair avec sa key et son val.
 * 
 * returns: l'element kvp.
 */

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



/*
 * Function: freeKeyVal
 * =======================
 * Libère l'element (kv) de type key_value_pair
 * 
 * returns: void.
 */

void freeKeyVal(kvp *kv){
    if(kv->key) free(kv->key);
    if(kv->value) free(kv->value);
    free(kv);
}



/*
 * Function : kvts
 * ==================
 * Convertit un element (k) en une chaine de caracteres de la forme "key:value".
 * 
 * returns: le chaine representant l'element.
 */

char* kvts(kvp *k){
    int size = 2;
    if(k->key) size += strlen(k->key);
    if(k->value) size += strlen(k->value);
    char *str = (char*)malloc(sizeof(char) * size);

    snprintf(str, size, "%s:%s", k->key ? k->key : "", k->value ? k->value : "");
    return str;
}




/*
 * Function : stkv
 * =================
 * Convertit une chaine de caracteres (str) representant un element en un element.
 * 
 * returns: l'element represente par la chaine.
 */

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



/*
 * Function: initCommit
 * =======================
 * Alloue et initialise un Commit de taille fixee par la constante MAX_SIZE_COMMIT.
 * 
 * returns: le Commit initialise.
 */

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



/*
 * Function: freeCommit
 * =======================
 * Libère le commit (c).
 * 
 * returns: void.
 */

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
 * Function: hash_sdbm
 * ======================
 * Fonction de hashage choisi : sdbm
 * Il est efficace pour brouiller les bits, entrainant une meilleure distribution des cles et moins de fractionnements.
 * Il s'agit egalement d'une bonne fonction de hachage generale avec une bonne distribution.
 * 
 * returns: unsigned long etant le hash de la chaine de caractere (str).
 */

unsigned long hash_sdbm(char *str){
    unsigned long hash = 0;
    int c;
    while((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}



/*
 * Function: commitSet
 * ======================
 * Insere la paire (key, value) dans le commit (c), en gerant les collisions par adressage ouvert et probing lineaire.
 * 
 * returns: void.
 */

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
 * Function: commitSet
 * ======================
 * Insere l'element (kv) dans le commit (c), en gerant les collisions par adressage ouvert et probing lineaire.
 * 
 * note: Utilise uniquement dans la fonction stc. C'est la meme fonction que commitSet, a la difference qu'on a pas a alloue un nouvel element.
 * 
 * returns: void.
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



/*
 * Function: createCommit
 * =========================
 * Alloue et initialise un Commit, puis ajoute l'element obligatoire (hash) correspondant a la cle "tree".
 * 
 * returns: le Commit initialise.
 */

Commit* createCommit(char *hash){
    Commit *c = initCommit();
    commitSet(c, "tree", hash);
    return c;
}



/*
 * Function: commitGet
 * ======================
 * Cherche dans le commit (c) s'il existe un element dont la cle est key (en sachant que les conflits sont resolus par adressage ouvert et probing lineaire).
 * 
 * returns: La valeur de l'element s'il existe.
 *          Sinon NULL.
 */

char* commitGet(Commit *c, char *key){
    unsigned long hash = hash_sdbm(key) % c->size;

    for(int i = 0; i < c->size; i++){
        int index = (hash + i) % c->size;

        if(c->T[index] == NULL) 
            return NULL;
        
        if(strcmp(c->T[index]->key, key) == 0)
            return strdup(c->T[index]->value);
    }

    return NULL;
}



/*
 * Function: tailleCommitHash_STR
 * =================================
 * Renvoie la taille necessaire pour stocker dans une chaine de caractere les kvp du commit (c).
 *      - chaine du type "key1:value1\nkey2:value2\n..."
 * 
 * note: rajout de cette fonction non-demandee pour la fonction cts.
 * 
 * returns: la taille.
 */

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



/*
 * Function: cts
 * ================
 * Convertit le commit (c) en une chaine de caracteres composee des chaines representant chacun de ses couples (key, value) separees par un saut de ligne.
 * 
 * returns: la chaine de caracteres du type "key1:value1\nkey2:value2\n..."
 */

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



/*
 * Function: stc
 * ================
 * Convertit une chaine de caracteres (ch) representant un Commit en Commit.
 * 
 * returns: le Commit represente par la chaine.
 */

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



/*
 * Function: ctf
 * ================
 * Ecrit dans le fichier (file) la chaine de caractere representant le commit (c).
 * 
 * returns: void.
 */

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



/*
 * Function: ftc
 * ================
 * Charge un commit grace au fichier (file) le representant.
 * 
 * returns: Le commit represente par file.
 */

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



/*
 * Function: blobCommit
 * =======================
 * Cree un fichier temporaire representant le commit (c) pour pouvoir ensuite creer l'enregistrement instantane du commit avec l'extension ".c".
 * 
 * note: meme procede que pour la fonction blobWorkTree.
 * 
 * returns: le hash du fichier temporaire.
 */

char* blobCommit(Commit *c){
    /*
     * Création du fichier temporaire afin d'avoir la hash du WorkTree.
     */
    if(!file_exists(".tmp")){
        system("mkdir .tmp");
    }

    static char template[] = "./.tmp/blobCommit_XXXXXX";
    char fname[1000];
    strcpy(fname, template);
    int fd = mkstemp(fname);
    ctf(c, fname);
    close(fd);

    
    /*
     * Même fonction que BlobFile, à la différence près qu'on rajoute ".c" à la fin du nom de l'instantané.
     */
    char *hash = sha256file(fname);
    char rep[13];
    snprintf(rep, 13, ".autosave/%c%c", hash[0], hash[1]);

    char linuxCommand[1024];
    if(!file_exists(".autosave")){
        system("mkdir .autosave");
    }
    if(!file_exists(rep)){
        snprintf(linuxCommand, 22, "mkdir -p .autosave/%c%c", hash[0], hash[1]);
        system(linuxCommand);
    }

    char *path = hashToPath(hash);

    int size_HFN = strlen(path)+13;
    char *hashFileName = malloc(sizeof(char)*(size_HFN));
    if (hashFileName == NULL){
        printf("Erreur : allocation de HashFileName (BlobCommit)\n");
        exit(1);
    }

    snprintf(hashFileName, size_HFN, ".autosave/%s.c", path);
    cp(hashFileName,fname);
    free(hashFileName);
    free(path);


    /*
     * On supprime le fichier temporaire.
     */
    snprintf(linuxCommand, 1024, "rm -f %s", fname+2);
    system(linuxCommand);

    return hash;
}



//=====================================================================



/*
 * Function: initRefs
 * =====================
 * Cree le repertoire cache .refs s'il n'existe pas deja, puis cree les fichiers master et HEAD (vides).
 * 
 * returns: void.
 */

void initRefs(){
    if (!file_exists(".refs")){
        system("mkdir .refs");
        system("touch .refs/master");
        system("touch .refs/HEAD");
    }
}



/*
 * Function: createUpdateRef
 * ============================
 * Met a jour une reference (ref_name) en remplacant son contenu par hash.
 * Si la reference n'existe pas la fonction commence par creer le fichier.
 * 
 * returns: void.
 */

void createUpdateRef(char *ref_name, char *hash){
    initRefs(); // Vérifie si le répertoire .refs/ existe. Le crée si besoin.

    char *buff = (char *)malloc(sizeof(char) * (strlen(ref_name) + 7));
    if(buff == NULL){
        printf("Erreur : allocation buff (createUpdateRef)\n");
        exit(1);
    }

    sprintf(buff, ".refs/%s", ref_name);

    FILE *f = fopen(buff, "w");
    if(f == NULL){
        printf("Erreur : Ouverture de %s (createUpdateRef)\n", buff);
        free(buff);
        exit(1);
    }

    fprintf(f, "%s", hash ? hash : "");
    fclose(f);
    free(buff);
}



/*
 * Function: deleteRef
 * ======================
 * Supprime la reference (ref_name) si elle existe.
 * 
 * returns: void.
 */

void deleteRef(char *ref_name){
    char *buff = (char *)malloc(sizeof(char) * (strlen(ref_name) + 10));
    if(buff == NULL){
        printf("Erreur : allocation buff (deleteRef)\n");
        exit(1);
    }

    sprintf(buff, ".refs/%s", ref_name);

    if(file_exists(buff)){
        sprintf(buff, "rm .refs/%s", ref_name);
        system(buff);
    } else {
        printf("La référence %s n'existe pas.\n", ref_name);
    }

    free(buff);
}



/*
 * Function: getRef
 * ===================
 * Recupere vers quoi pointe la reference (ref_name), c'est a dire le hash contenu dans le fichier.
 * 
 * returns: Le hash pointe par la reference.
 *          "" s'il la reference est vide.
 *          NULL si la reference n'existe pas.
 */

char* getRef(char *ref_name){
    char *buff = (char *)malloc(sizeof(char) * (strlen(ref_name) + 7));
    if(buff == NULL){
        printf("Erreur : allocation buff (getRef)\n");
        exit(1);
    }

    sprintf(buff, ".refs/%s", ref_name);

    if(!file_exists(buff)){
        printf("La référence %s n'existe pas.\n", ref_name);
        free(buff);
        return NULL;
    }


    char *hash = (char *)malloc(sizeof(char) * 256);
    if(hash == NULL){
        printf("Erreur : allocation hash (getRef)\n");
        free(buff);
        exit(1);
    }

    FILE *f = fopen(buff, "r");
    if(f == NULL){
        printf("Erreur : ouverture de %s (getRef)\n", buff);
        free(buff); free(hash);
        exit(1);
    }


    if(fgets(hash, 256, f) == NULL){
        fclose(f);
        free(buff);
        free(hash);
        return NULL;
    }

    fclose(f);
    free(buff);
    return hash;
}



//=====================================================================



/*
 * Function: myGitAdd
 * =====================
 * Permet a l'utilisateur d'ajouter un fichier ou un repertoire (file_or_folder) dans le WorkTree correspondant à la zone de preparation (ici le .add).
 * Si le fichier ".add" n'existe pas, il le cree.
 * 
 * returns: void.
 */

void myGitAdd(char *file_or_folder){
    WorkTree *wt;
    if(file_exists(".add")){
        wt = ftwt(".add");
    } else {
        system("touch .add");
        wt = initWorkTree();
    }

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