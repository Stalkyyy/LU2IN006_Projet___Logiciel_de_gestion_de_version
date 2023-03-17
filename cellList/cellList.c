#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "cellList.h"

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

    c->data = strdup(ch);
    c->next = NULL;
    return c;
}


void insertFirst(List *l, Cell* c){
    c->next = *l;
    *l = c;
}

/*
    Rajout de cette fonction pour stol.
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
    Renvoie la somme des tailles des chaines de caractères de chaque cellule composant la liste L.
    La taille comprend le caractère de fin '\0'. 
        En effet, dans notre fonction ltos(), on le remplacera par le caractère '|', ou on gardera '\0'.
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
    Modification, nous avons rajouté un délimiteur dans les paramètres, étant donné que l'énoncé ne donne aucune précision de comment découper la chaine de caractères.
    De plus, on utilise la fonction insertLast() afin que le premier élément de la chaine s soit en première position de la liste.
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
            insertFirst(l, buildCell(ep->d_name));
        }
    }

    closedir(dp);
    return l;
}

int file_exists(char *file){
    char *cur_dir;
    cur_dir=(char *)malloc(100*sizeof(char));
    getcwd(cur_dir,100);
    DIR* dp = opendir((char*)cur_dir);
    struct dirent *ep;
    if(dp != NULL){
        while((ep = readdir(dp)) != NULL)
        {
            if(strcmp(file, ep->d_name) == 0){
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
        printf("Le ficiher source n'existe pas.\ns");
        exit(1);
    }

    char* contenu = malloc(sizeof(char)*1000);

    FILE *fsource = fopen(from, "r");
    if (fsource == NULL){
        printf("Erreur : ouverture de %s\n", to);
        exit(1);
    }

    FILE *fdest = fopen(to, "w");


    while(fgets(contenu, 1000, fsource) != NULL){
        fputs(contenu, fdest);
    }

    free(contenu);
    fclose(fsource);
    fclose(fdest);
}

char* hashToPath(char* hash){
    int i;
    int length = strlen(hash);
    char* first = malloc(3*sizeof(char));
    char* second = malloc((length - 1)*sizeof(char)); 


    for(i = 0; i < 2; i++){
        first[i] = hash[i];
    }
    first[i] = '\0';
    int j = 0;
    for(i = 2; i<length; i++){
        second[j] = hash[i];
        j++;
    }
    second[j] = '\0';

    char* path = malloc(sizeof(char)*(strlen(first)+strlen(second)+2));
    strcpy(path, first);
    strcat(path, "/");
    strcat(path, second);
    free(first);
    free(second);
    return path;
}


void blobFile(char* file){
    if(!file_exists("autosave")){
         system("mkdir autosave");
    }
    char* new = malloc(sizeof(char)* (strlen(file)+10));
    strcpy(new, "autosave/");
    strcat(new, file);
    cp(new,file);
    free(new);
}