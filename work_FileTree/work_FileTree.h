#ifndef _WORK_FILETREE_H_

#define MAX_TAB_WF 100

typedef struct{
    char *name;
    char *hash;
    int mode;
} WorkFile;


/*
 * Nous avons préféré faire un tableau de pointure de WorkFile ici.
 */
typedef struct{
    WorkFile **tab;
    int size;
    int n;
} WorkTree;

int getChmod(const char *path);
void setMode(int mode, char *path);

WorkFile* createWorkFile(char *name);
char* wfts(WorkFile *wf);
WorkFile* stwf(char *ch);
void freeWorkFile(WorkFile *wf);

WorkTree* initWorkTree();
int inWorkTree(WorkTree *wt, char *name);
int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode);
char* wtts(WorkTree *wt);
WorkTree* stwt(char *s);
int wttf(WorkTree *wt, char *file);
WorkTree* ftwt(char *file);
void freeWorkTree(WorkTree *wt);

char* blobWorkTree(WorkTree *wt);
char* saveWorkTree(WorkTree *wt, char *path);
void restoreWorkTree(WorkTree *wt, char *path);


#endif