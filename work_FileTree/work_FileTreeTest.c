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

    int n2 = appendWorkTree(wt, "Test1", "bs/dbfjsdfsdbfjbfjsdf", 777);
    printf("%d : %s\t%s\t%d\n", n2, (wt->tab[0])->name, (wt->tab[0])->hash, (wt->tab[0])->mode);


    appendWorkTree(wt, "Test2", "bt/ksjdnfskdjfnsjdkfnf", 543);
    appendWorkTree(wt, "Test3", "bt/ksjdnfdgdfgkdjfnsjdkfnf", 343);
    appendWorkTree(wt, "Test4", "bt/ksjdnfdgdfgkdjfnsjdkfnf", 343);
    appendWorkTree(wt, "Test5", "bt/ksjdnfdgdfgkdjfnsjdkfnf", 343);
    appendWorkTree(wt, "Test6", "bt/ksjdnfdgdfgkdjfnsjdkfnf", 343);


    char* str = wtts(wt);
    printf("\n%s\n", str);

    char str2[255] = "Test7\tbt/ksjdnfskdjfnsjdkfnf\t543\nTest8\tbt/ksjdnfskdjfnsjdkfnf\t544\nTest9\tbt/ksjdnfskdjfnsjdkfnf\t545\nTest10\tbt/ksjdnfskdjfnsjdkfnf\t546\nTest11\tbt/ksjdnfskdjfnsjdkfnf\t547\nTest12\tbt/ksjdnfskdjfnsjdkfnf\t548\n";

    WorkTree *wt2 = stwt(str2);

    for(int i = 0; i < wt2->n; i++){
        printf("%s\t%s\t%d\n", wt2->tab[i]->name, wt2->tab[i]->hash, wt2->tab[i]->mode);
    }

    wttf(wt2, "test_wttf.txt");

    printf("\n");
    WorkTree *wt3 = ftwt("test_ftwt.txt");
    for(int i = 0; i < wt3->n; i++){
        printf("%s\t%s\t%d\n", wt3->tab[i]->name, wt3->tab[i]->hash, wt3->tab[i]->mode);
    }

    free(str);
    freeWorkTree(wt3);
    freeWorkTree(wt2);
    freeWorkTree(wt);

    return 0;
}