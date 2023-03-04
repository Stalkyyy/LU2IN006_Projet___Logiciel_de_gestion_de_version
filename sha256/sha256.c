#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sha256.h"

int hashFile(char* source, char* dest){
    char linuxCommand[256];

    /*
        Ecriture de la commande utilisée.
        Ici, on appelle sha256sum sur le fichier source. 
        Ensuite on coupe la sortie, on ne veut que le hash, pas le nom du fichier !
    */
    snprintf(linuxCommand, 256, "sha256sum %s | cut -d ' ' -f 1 > %s", source, dest);
    
    if (system(linuxCommand) != 0){
        printf("Erreur : Problème d'exécution de la commande.\n");
        exit(1);
    }

    return 0;
}


char* sha256file(char* file){
    /* 
        Création du fichier temporaire 
    */
    static char template[] = "/tmp/tmp_XXXXXX";
    char tempName[1000];
    strcpy(tempName, template);
    mkstemp(tempName);


    /*
        Création de la commande Linux et ensuite son exécution.
    */
    char linuxCommand[2048];
    snprintf(linuxCommand, 2048, "sha256sum %s | cut -d ' ' -f 1 > %s", file, tempName);

    if (system(linuxCommand) != 0){
        printf("Erreur : Problème d'exécution de la commande.\n");
        exit(1);
    }


    FILE *tmp = fopen(tempName, "r");
    if (tmp == NULL){
        printf("Erreur : ouverture du fichier temporaire.\n");
        exit(1);
    }

    char *hash = (char *)(malloc(sizeof(char)*SHA256_LENGTH + 1));
    if(fgets(hash, SHA256_LENGTH + 1, tmp) == NULL){
        printf("Erreur : lecture du fichier temporaire.\n");
        exit(1);
    }


    /*
        Supression du fichier temporaire.
    */
    fclose(tmp);
    snprintf(linuxCommand, 1024, "rm %s", tempName);
    if (system(linuxCommand) != 0)
        printf("Warning : Supression du fichier temporaire ratée.\n");

    return hash;
}