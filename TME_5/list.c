#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct cell {
    char * data ;
    struct cell * next ;
} Cell ;


typedef Cell * List ;

List* initList(){
    List* new = (List*)malloc(sizeof(List));
    if (new == NULL)
        exit(1);

    return new;
}

Cell* buildCell(char* ch){
    Cell* c = (Cell*)malloc(sizeof(Cell));
    if (c == NULL)
        exit(1);

    c->data = strdup(ch);
    c->next = NULL;
    return c;
}

void insertFirst(List* L, Cell* C){
    C->next = *L;
    *L = C;
}

char* ctos(Cell* c){
    return c->data;
}

int tailleDataList(List *L){
    Cell* temp = *L;
    int taille;
    while(temp){
        taille += strlen(temp->data)+1;
        temp = temp->next;
    }

    return taille +1;

}

char* ltos(List* L){
    Cell* temp = *L;
  
    char* res = malloc(sizeof(char)*tailleDataList(L));

    while(temp->next){
        strcat(res, ctos(temp));
        strcat(res, "|");
        temp = temp->next;
    }

    strcat(res, ctos(temp));

    return res;
}


Cell* listGet(List* L, int i){
    Cell* temp = *L;
    int j = 0;
    while(j < i && temp != NULL){
        temp = temp->next;
        j++;
    }

    return temp;
}

Cell* searchList(List* L, char* str){
    Cell* temp = *L;
    while(temp){
        if(strcmp(temp->data, str) == 0){
            return temp;
        }
        temp = temp->next;
        
    }

    return NULL;
}

/*
List* stol(char* s){
    List* l = initList();

    char *test = strdup(s);
    const char * seperator = "|";
    char* strToken = strtok(test, seperator);
    while(strToken != NULL){
        insertFirst(l, buildCell(strToken));
        strToken = strtok(test, seperator);
    }

    return l;
}
*/

void ltof(List* L, char* path){
    FILE *f = fopen(path, "w");

    Cell *temp = *L;
    while(temp){
        fprintf(f, "%s\n", temp->data);
        temp = temp->next;
    }

    fclose(f);
}

List* ftol(char* path, int i){
    FILE *f = fopen(path, "r");

    List* l = initList();
    char data[256];

    int j = 0;
    while(j < i && EOF != fscanf(f, "%s", data)){
        insertFirst(l, buildCell(data));
        j++;
    }

    fclose(f);
    return l;
}

int main(){
    List *l = initList();
    char *s = "Coucou !";
    Cell *c = buildCell(s);
    Cell *c2 = buildCell("Test 2");
    insertFirst(l, c);
    insertFirst(l, c2);
    char* test = ltos(l);
    printf("%s\n", test);

    ltof(l, "test.txt");
    List* testL = ftol("test.txt", 1);

    Cell *c3 = *testL;
    while(c3){
        printf("%s\n", c3->data);
        c3 = c3->next;
    }

    return 0;
}