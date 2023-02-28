#ifndef _CELLLIST_H_

typedef struct cell {
    char *data;
    struct cell *next;
} Cell;
typedef Cell* List;

List* initList();
Cell* buildCell(char* ch);
void insertFirst(List *L, Cell *C);
char* ctos(Cell *C);
char* ltos(List *L);
Cell* listGet(List* L, int i);
Cell* searchList(List *L, char *str);
List* stol(char *s);
void ltof(List *L, char *path);
List* ftol(char *path);

#endif