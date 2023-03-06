#ifndef _WORK_FILETREE_H_

#define MAX_TAB_WF 100

typedef struct{
    char *name;
    char *hash;
    int mode;
} WorkFile;

typedef struct{
    WorkFile *tab;
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


#endif