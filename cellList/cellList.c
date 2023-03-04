#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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