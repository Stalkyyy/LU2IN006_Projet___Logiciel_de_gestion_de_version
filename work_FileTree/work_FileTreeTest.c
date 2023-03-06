#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "work_FileTree.h"
#include "../sha256/sha256.h"
#include "../cellList/cellList.h"

int main(){
    WorkFile *wf = createWorkFile("../cellList/cellList.c");

    wf->hash = strdup("lk/fhsjglkdfqsjglsdf");
    wf->mode = 755;
    char* test = wfts(wf);
    printf("%s\n", test);

    WorkFile *wf2 = stwf(test);
    printf("%s, %s, %d\n", wf2->name, wf2->hash, wf2->mode);

    free(test);
    freeWorkFile(wf);
    freeWorkFile(wf2);


    //================================================================


    WorkTree *wt = initWorkTree();
    int n1 = inWorkTree(wt, "test.txt");
    printf("%d\n", n1);

    int n2 = appendWorkTree(wt, "TestWorkTree", "bs/dbfjsdfsdbfjbfjsdf", 777);
    printf("%d : %s\t%s\t%d\n", n2, (wt->tab[0]).name, (wt->tab[0]).hash, (wt->tab[0]).mode);

    freeWorkTree(wt);

    return 0;
}