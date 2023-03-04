#ifndef _CELLLIST_H_

typedef struct cell {
    char *data;
    struct cell *next;
} Cell;
typedef Cell* List;

List* initList();
Cell* buildCell(char* ch);

void insertFirst(List *L, Cell *C);
void insertLast(List *l, Cell *c);

char* ctos(Cell *C);
char* ltos(List *L);
Cell* listGet(List* L, int i);

int sizeCharList(List* L);
Cell* searchList(List *L, char *str);

List* stol(char *s, char* delim);
void ltof(List *L, char *path);
List* ftol(char *path);

void insertLast(List *l, Cell *c);
void freeList(List *l);

#endif