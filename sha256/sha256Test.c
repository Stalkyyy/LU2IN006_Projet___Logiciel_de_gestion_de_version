#include <stdio.h>
#include <stdlib.h>
#include "sha256.h"

int main(){
    hashFile("sha256Test.c", "hash_sha256Test.txt");


    char *hash = sha256file("sha256Test.c");
    printf("%s\n", hash);

    free(hash);
    return 0;
}