#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "cellList.h"
#include "../sha256/sha256.h"

List* initList(){
    List* l = (List *)malloc(sizeof(List));
    if (l == NULL){
        printf("Erreur : initList() erreur allocution.\n");
        exit(1);
    }

    *l = NULL;
    return l;
}


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


void insertFirst(List *l, Cell* c){
    c->next = *l;
    *l = c;
}

/*
 *Rajout de cette fonction pour stol.
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


char* ctos(Cell *c){
    return c->data;
}


/*
 * Renvoie la somme des tailles des chaines de caractères de chaque cellule composant la liste L.
 * La taille comprend le caractère de fin '\0'. 
 * En effet, dans notre fonction ltos(), on le remplacera par le caractère '|', ou on gardera '\0'.
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


Cell* listGet(List* L, int i){
    Cell *c = *L;
    int j = 1;

    while(j < i && c != NULL){
        c = c->next;
        j++;
    }

    return c;
}


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
 * Modification, nous avons rajouté un délimiteur dans les paramètres, étant donné que l'énoncé ne donne aucune précision de comment découper la chaine de caractères.
 * De plus, on utilise la fonction insertLast() afin que le premier élément de la chaine s soit en première position de la liste.
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

int file_exists(char *file){
    char *cur_dir;
    cur_dir=(char *)malloc(1000*sizeof(char));
    getcwd(cur_dir,1000);
    
    char fileName[strlen(file)+1];
    strcpy(fileName, file);
    char *dirFile = strrchr(file, '/');

    /*
     * Ici, on crée une chaine de caractère ayant le pwd + le chemin vers un fichier. 
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

char* hashToPath(char* hash){
    char *path = (char*)malloc(sizeof(char)*(strlen(hash)+2));
    if(path == NULL){
        printf("Erreur : allocation de path (hashToPath())\n");
        exit(1);
    }

    snprintf(path, strlen(hash)+2, "%c%c/%s", hash[0], hash[1], hash + 2);
    return path;
}


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
    char* hashFileName = malloc(sizeof(char)*(strlen(path)+11));

    snprintf(hashFileName, strlen(path)+11, ".autosave/%s", path);
    cp(hashFileName,file);


    free(hashFileName);
    free(path);
    free(hash);
}