#include <stdio.h>
#include <stdlib.h>
#include "./sha256/sha256.h"
#include "./cellList/cellList.h"

int main(){
    char *hash = sha256file("main.c");
    printf("%s\n", hash);

    free(hash);
    return 0;
}