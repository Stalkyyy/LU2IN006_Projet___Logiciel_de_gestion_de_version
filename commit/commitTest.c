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
    kvp* kv = createKeyVal("key1", "val1");
    char* test = kvts(kv);
    printf("%s\n", test);
    

    kvp* kv2 = stkv(test);
    printf("%s, %s\n", kv2->key, kv2->value);
    free(test);

    freeKeyVal(kv);
    freeKeyVal(kv2);

    Commit* c = initCommit();
    
    commitSet(c, "key1", "val1");
    commitSet(c, "key2", "val2");
    commitSet(c, "key3", "val3");
    commitSet(c, "key4", "val4");
    commitSet(c, "key5", "val5");

    affichageCommit(c);

    char* test2 = cts(c);
    printf("%s\n", test2);
    Commit* c2 = stc(test2);
    affichageCommit(c2);
    
    printf("%s\n\n", commitGet(c2, "key3"));

    ctf(c, "ctf.txt");

    Commit* c3 = ftc("ctf.txt");
    affichageCommit(c3);

    char* hash = blobCommit(c3);

    free(test2);
    free(hash);
    freeCommit(c);
    freeCommit(c2);
    freeCommit(c3);


    initRefs();
    createUpdateRef("master", "gkjdshgkjfdk");
    char* ref = getRef("sfjgldfj");
    if(ref != NULL)
        printf("%s\n", ref);
    free(ref);
    deleteRef("HEAD");

}