#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cellList.h"

List* initList(){
    List* l = (List *)malloc(sizeof(List));
    if (l == NULL){
        printf("Erreur : initList() erreur allocution.");
        exit(1);
    }

    return l;
}