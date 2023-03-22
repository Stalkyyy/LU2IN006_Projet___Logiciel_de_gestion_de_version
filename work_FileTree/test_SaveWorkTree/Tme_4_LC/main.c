#include <stdio.h>
#include <stdlib.h>
#include "biblioH.h"
#include "entreeSortieH.h"

int main(){
    BiblioH* b = charger_n_entrees("GdeBiblio.txt", 10);
    enregistrer_biblio(b, "test.txt");

    liberer_biblio(b);
}