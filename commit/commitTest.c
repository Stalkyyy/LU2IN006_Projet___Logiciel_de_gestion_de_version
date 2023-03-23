#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "commit.h"
#include "../sha256/sha256.h"
#include "../cellList/cellList.h"
#include "../work_FileTree/work_FileTree.h"

int main(){
    kvp *kv1 = createKeyVal("Test1", "Réussi ? 1");
    printf("%s => %s\n", kv1->key, kv1->value);
    freeKeyVal(kv1);

    kvp *kv2 = createKeyVal("Test2", "Réussi ? 2");
    char *str2 = kvts(kv2);
    printf("%s\n", str2);
    freeKeyVal(kv2);
    free(str2);

    kvp *kv3 = stkv("Test3:Réussi ? 3");
    printf("%s => %s\n\n", kv3->key, kv3->value);
    freeKeyVal(kv3);

    //=====================================================================

    Commit *c1 = createCommit("b4/46e2d4fa671ad08c614c93302d07642dc201a32d7621e0503a09433fd649b7.t");
    commitSet(c1, "Test_1_c1", "Réussite_1_c1");
    commitSet(c1, "Test_2_c1", "Réussite_2_c1");
    commitSet(c1, "Test_3_c1", "Réussite_3_c1");

    char *c1_getTree = commitGet(c1, "tree");
    char *c1_get1 = commitGet(c1, "Test_1_c1");
    char *c1_get2 = commitGet(c1, "Test_2_c1");
    char *c1_get3 = commitGet(c1, "Test_3_c1");
    char *c1_getNULL = commitGet(c1, "Test_NULL_c1");

    for(int i = 0; i < c1->size; i++){
        if(c1->T[i] != NULL)
            printf("(test c1) %s => %s\n", c1->T[i]->key, c1->T[i]->value);
    }

    printf("\n");

    c1_getTree ? printf("(test commitGet) tree => %s\n", c1_getTree) : printf("tree inexistant.\n");
    c1_get1 ? printf("(test commitGet) Test_1_c1 => %s\n", c1_get1) : printf("Test_1_c1 inexistant.\n");
    c1_get2 ? printf("(test commitGet) Test_2_c1 => %s\n", c1_get2) : printf("Test_2_c1 inexistant.\n");
    c1_get3 ? printf("(test commitGet) Test_3_c1 => %s\n", c1_get3) : printf("Test_3_c1 inexistant.\n");
    c1_getNULL ? printf("(test commitGet) Test_NULL_c1 => %s\n", c1_getNULL) : printf("Test_NULL_c1 inexistant.\n");

    char *str_c1 = cts(c1);
    printf("\nTest cts(c1) :\n%s", str_c1);

    free(str_c1);
    freeCommit(c1);

    printf("\n");

    char str_c2[88] = "Test_1_c2:Réussi ? 1\nTest_2_c2:Réussi ? 2\nTest_3_c2:Réussi ? 3\nTest_4_c2:Réussi ? 4\n";
    Commit *c2 = stc(str_c2);

    for(int i = 0; i < c2->size; i++){
        if(c2->T[i] != NULL)
            printf("(test c2, stc(str_c2)) %s => %s\n", c2->T[i]->key, c2->T[i]->value);
    }

    ctf(c2, "test_ctf.txt");
    freeCommit(c2);

    printf("\n");

    Commit *c3 = ftc("test_ftc.txt");
    for(int i = 0; i < c3->size; i++){
        if(c3->T[i] != NULL)
            printf("(test ftc(test_ftc.txt)) %s => %s\n", c3->T[i]->key, c3->T[i]->value);
    }

    char *hash = blobCommit(c3);

    free(hash);
    freeCommit(c3);
    return 0;
}