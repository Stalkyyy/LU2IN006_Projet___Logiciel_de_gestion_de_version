#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "cellList.h"
#include "../sha256/sha256.h"


/*
 * Function: initList
 * =======================
 * Initialise une liste vide.
 * 
 * returns: la nouvelle liste initialisee vide.
 */

List* initList(){
    List* l = (List *)malloc(sizeof(List));
    if (l == NULL){
        printf("Erreur : initList() erreur allocution.\n");
        exit(1);
    }

    *l = NULL;
    return l;
}



/*
 * Function: buildCell
 * ======================
 * Initialise une nouvelle cellule avec son data (ch) en parametre.
 * 
 * ch: Chaine de caractere supposee être le data de la cellule.
 *     Si la chaine est nulle, alors le data le sera aussi.
 * 
 * returns: La nouvelle cellule initialisee.
 */

Cell* buildCell(char* ch){
    Cell *c = (Cell *)malloc(sizeof(Cell));
    if (c == NULL){
        printf("Erreur : buildCell() erreur allocution.\n");
        exit(1);
    }

    c->data = ch ? strdup(ch) : NULL;
    c->next = NULL;
    return c;
}



/*
 * Function: insertFirst
 * ========================
 * Insere en debut de liste (l) une cellule (c).
 * 
 * returns: void.
 */

void insertFirst(List *l, Cell* c){
    c->next = *l;
    *l = c;
}



/*
 * Function: insertLast
 * ========================
 * Insere en fin de liste (l) une cellule (c).
 * 
 * note: Cette fonction n'est pas demandee, on l'a faite pour la fonction stol. Nous ne savions pas si l'ordre importait a l'epoque.
 * 
 * returns: void.
 */

void insertLast(List *l, Cell *c){
    Cell *c_list = *l;

    if(c_list == NULL)
        *l = c;
    else{
        while(c_list->next != NULL){
            c_list = c_list->next;
        }
        c_list->next = c;
    }
}



/*
 * Function: ctos
 * =================
 * Retourne le data de la cellule (c) si elle n'est pas nulle. Sinon on renvoie NULL.
 */
char* ctos(Cell *c){
    if(c != NULL)
        return c->data;
    return NULL;
}



/*
 * Function: sizeCharList
 * =========================
 * Permet de recuperer la taille necessaire pour la chaine de la fonction ltos.
 * 
 * returns: Renvoie la somme des tailles des data de chaque cellule de la liste L.
 *          La taille comprend le caractere de fin '\0' de chaque data.
 *          En effet, dans notre fonction ltos, on le remplacera par le caractere '|' ou on le gardera.
 */

int sizeCharList(List* L){
    Cell *c = *L;
    int size = 0;
    
    while(c){
        size += strlen(c->data) + 1;
        c = c->next;
    }

    return size;
}



/*
 * Function: ltos
 * =================
 * Transforme une liste (L) de chaine de caractere dans le format suivant : chaine1|chaine2|...
 * 
 * returns: la chaine de caractere chaine1|chaine2|... suivant le nombre de data present.
 */

char* ltos(List* L){
    Cell *c = *L;
    char *str = (char *)malloc(sizeof(char)*sizeCharList(L));
    strcpy(str, "");

    while(c->next){
        strcat(str, ctos(c));
        strcat(str, "|");
        c = c->next;
    }

    strcat(str, ctos(c));
    return str;
}



/*
 * Function: listGet
 * ====================
 * Renvoie le i-eme element d'une liste (L).
 * 
 * returns: La cellule representant le i-eme element de la liste. S'il n'existe pas, NULL.
 */

Cell* listGet(List* L, int i){
    Cell *c = *L;
    int j = 1;

    while(j < i && c != NULL){
        c = c->next;
        j++;
    }

    return c;
}



/*
 * Function: searchList
 * =======================
 * Recherche un element dans une liste (L) a partir de son contenu (str).
 * 
 * returns: Une reference vers la cellule si l'element est trouvee.
 *          Sinon, NULL.
 */

Cell* searchList(List *L, char *str){
    Cell *c = *L;

    while(c){
        if(strcmp(c->data, str) == 0)
            return c;
        c = c->next;
    }

    return NULL;
}



/*
 * Function: stol
 * =================
 * Transforme une chaine de caracteres de la forme chaine1|chaine2|... (s) representant une liste en une liste chainee.
 * 
 * note : Nous avons rajoute un delimiteur dans les parametres, etant donne que l'enonce ne donne aucune precision de comment decouper la chaine de caracteres.
 *        On utilise la fonction insertLast() afin que le premier element de la chaine s soit en premiere position de la liste.
 * 
 * returns: la liste representant le chaine de caractere.
 */

List* stol(char* s, char* delim){
    List *l = initList();
    char *ptr = strtok(s, delim);

    while(ptr != NULL){
        Cell *c = buildCell(ptr);
        insertLast(l, c);
        ptr = strtok(NULL, delim);
    }

    return l;
}



/*
 * Function: ltof
 * =================
 * Ecris une liste (L) dans un fichier (path).
 * 
 * returns: void.
 */

void ltof(List *L, char *path){
    FILE *f = fopen(path, "w");
    if (f == NULL){
        printf("Erreur : ouverture de %s\n", path);
        exit(1);
    }

    Cell *c = *L;
    while(c){
        fprintf(f, "%s\n", c->data);
        c = c->next;
    }

    fclose(f);
}



/*
 * Function: ftol
 * =================
 * Creer une liste representant la liste enregistree dans le fichier (path).
 * 
 * returns: la liste representee par le fichier (path).
 */

List* ftol(char *path){
    FILE *f = fopen(path, "r");
    if (f == NULL){
        printf("Erreur : ouverture de %s\n", path);
        exit(1);
    }

    List *l = initList();
    char data[256];

    while(EOF != fscanf(f, "%s", data)){
        Cell *c = buildCell(data);
        insertLast(l, c);
    }

    fclose(f);
    return l;
}



/*
 * Function: freeList
 * =====================
 * Libere la liste (l).
 * 
 * returns: void.
 */

void freeList(List *l){
    Cell *c = *l;
    Cell *temp;
    while(c != NULL){
        temp = c->next;
        free(c->data);
        free(c);
        c = temp;
    }

    free(l);
}


//==================================================================================================


/*
 * Function: listdir
 * ====================
 * Renvoie une liste contenant les noms des fichiers et repertoires presents dans le chemin (root_dir).
 * 
 * returns: une liste de fichiers et repertoires.
 */

List* listdir(char* root_dir){
    DIR* dp = opendir(root_dir);
    struct dirent *ep;
    List* l = initList();
    if(dp != NULL)
    {
        while ((ep = readdir(dp)) != NULL)
        {
            /*
             * On teste si on a pas les dossiers ./ et ../
             * En effet, ça les rajouterait dans la liste.
            */
            if(strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0)
                insertFirst(l, buildCell(ep->d_name));
        }
    }

    closedir(dp);
    return l;
}



/*
 * Function: file_exists
 * ========================
 * Verifie si le fichier (file) existe dans le repertoire courant.
 * 
 * returns: 1 s'il existe, 0 sinon.
 */

int file_exists(char *file){
    char *cur_dir;
    cur_dir=(char *)malloc(1000*sizeof(char));
    getcwd(cur_dir,1000);
    
    char fileName[strlen(file)+1];
    strcpy(fileName, file);
    char *dirFile = strrchr(file, '/');

    /*
     * Ici, on cree une chaine de caractere ayant le pwd + le chemin vers un fichier. 
     * Par exemple, si file == "/tmp/test/file.c", on ouvrira pwd + /tmp/test
     */
    if(dirFile != NULL){
        char sub_str[dirFile - file];
        strncpy(sub_str, file, dirFile - file);
        sub_str[dirFile - file] = '\0';

        strcat(cur_dir, "/");
        strcat(cur_dir, sub_str);
        strcpy(fileName, dirFile+1);
    }


    DIR* dp = opendir((char*)cur_dir);
    struct dirent *ep;
    if(dp != NULL){
        while((ep = readdir(dp)) != NULL)
        {
            if(strcmp(fileName, ep->d_name) == 0){
                free(cur_dir);
                closedir(dp);
                return 1;
            }
        }
    }

    closedir(dp);
    free(cur_dir);
    return 0;
}



/*
 * Function: cp
 * ===============
 * Copie le contenu du fichier (to) s'il existe dans le fichier (from).
 * 
 * returns: void.
 */

void cp(char *to, char *from){
    if(!file_exists(from)){
        printf("Le fichier source n'existe pas (%s)\n", from);
        exit(1);
    }

    char* contenu = malloc(sizeof(char)*1000);


    FILE *fsource = fopen(from, "r");
    if (fsource == NULL){
        printf("Erreur : ouverture de %s (cp)\n", from);
        exit(1);
    }

    FILE *fdest = fopen(to, "w");
    if (fsource == NULL){
        printf("Erreur : ouverture de %s (cp)\n", to);
        exit(1);
    }


    while(fgets(contenu, 1000, fsource) != NULL){
        fputs(contenu, fdest);
    }

    free(contenu);
    fclose(fsource);
    fclose(fdest);
}



/*
 * Function: hashToPath
 * =======================
 * Renvoie le chemin d'un fichier a partir de son hash (hash).
 * On rappelle que le chemin s'obtient en inserant un '/' entre le 2e et le 3e caractere du hash.
 * 
 * returns: la chaine de caractere du chemin qui represente le hash.
 */

char* hashToPath(char* hash){
    char *path = (char*)malloc(sizeof(char)*(strlen(hash)+2));
    if(path == NULL){
        printf("Erreur : allocation de path (hashToPath())\n");
        exit(1);
    }

    snprintf(path, strlen(hash)+2, "%c%c/%s", hash[0], hash[1], hash + 2);
    return path;
}



/*
 * Function: blobFile
 * =====================
 * Enregistre un instantane du fichier (file).
 * 
 * returns: void.
 */

void blobFile(char* file){
    char rep[13];
    char *hash = sha256file(file);
    snprintf(rep, 13, ".autosave/%c%c", hash[0], hash[1]);

    if(!file_exists(rep)){
        char linuxCommand[22];
        snprintf(linuxCommand, 22, "mkdir -p .autosave/%c%c", hash[0], hash[1]);
        system(linuxCommand);
    }

    char *path = hashToPath(hash);
    int size = strlen(path)+11;
    char* hashFileName = malloc(sizeof(char)*size);

    snprintf(hashFileName, size, ".autosave/%s", path);
    cp(hashFileName,file);


    free(hashFileName);
    free(path);
    free(hash);
}