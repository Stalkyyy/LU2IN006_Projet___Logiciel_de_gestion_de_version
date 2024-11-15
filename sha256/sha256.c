#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sha256.h"
#include "../cellList/cellList.h"

//============================================================================================


/*
 * Function: hashFile
 * =====================
 * Recupere le hash du fichier (source) et l'ecrit dans le fichier (dest).
 * 
 * returns: 0 si tout se passe bien.
 */

int hashFile(char* source, char* dest){
    char linuxCommand[256];

    /*
     * Ecriture de la commande utilisee.
     * Ici, on appelle sha256sum sur le fichier source. 
     * Ensuite on coupe la sortie, on ne veut que le hash, pas le nom du fichier !
     */
    snprintf(linuxCommand, 256, "sha256sum %s | cut -d ' ' -f 1 > %s", source, dest);
    
    if (system(linuxCommand) != 0){
        printf("Erreur : Problème d'exécution de la commande.\n");
        exit(1);
    }

    return 0;
}


//============================================================================================


/*
 * Function: sha256file
 * =======================
 * Renvoie le hash du fichier (file).
 * 
 * returns: Chaine de caractere (hash du fichier)
 */

char* sha256file(char* file){
    /* 
     * Creation du fichier temporaire 
     */

    if(!file_exists(".tmp")){
        system("mkdir .tmp");
    }

    static char template[] = "./.tmp/sha256_XXXXXX";
    char tempName[1000];
    strcpy(tempName, template);
    int fd = mkstemp(tempName);


    /*
     * Creation de la commande Linux et ensuite son execution.
     */
    char linuxCommand[2048];
    snprintf(linuxCommand, 2048, "sha256sum %s | cut -d ' ' -f 1 > %s", file, tempName);

    if (system(linuxCommand) != 0){
        printf("Erreur : Problème d'exécution de la commande.\n");
        exit(1);
    }

    char *hash = (char *)(malloc(sizeof(char)*SHA256_LENGTH + 1));
    if(read(fd, hash, SHA256_LENGTH) == -1){
        printf("Erreur : lecture du fichier temporaire.\n");
        exit(1);
    }
    hash[64] = '\0';

    close(fd);

    snprintf(linuxCommand, 2048, "rm -f %s", tempName+2);
    system(linuxCommand);

    return hash;
}