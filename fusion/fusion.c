#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../sha256/sha256.h"
#include "../cellList/cellList.h"
#include "../work_FileTree/work_FileTree.h"
#include "../commit/commit.h"
#include "../branch/branch.h"
#include "fusion.h"


/*
 * Function: mergeWorkTrees
 * ===========================
 * Etant donne les WorkTrees (wt1) et (wt2) :
 *      - cree une liste (conflicts) de chaines de caracteres composee des noms de fichiers/repertoires qui sont en conflit.
 *      - cree un nouveau WorkTree compose des fichiers/repertoires qui ne sont pas en conflit.
 * 
 * returns: le nouveau WorkTree (non-conflits).
 */

WorkTree* mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts){
    *conflicts = NULL; 
    WorkTree *newWT = initWorkTree();
    WorkFile *wf;

    int i;
    for(i = 0; i < wt1->n; i++){
        wf = wt1->tab[i];
        if (inWorkTree(wt2, wf->name) == -1){
            appendWorkTree(newWT, wf->name, wf->hash, wf->mode);
        } else {
            if(*conflicts == NULL) *conflicts = initList();
            insertFirst(*conflicts, buildCell(wf->name));
        }
    }

    for(i = 0; i < wt2->n; i++){
        wf = wt2->tab[i];
        if (inWorkTree(wt1, wf->name) == -1){
            appendWorkTree(newWT, wf->name, wf->hash, wf->mode);
        }
    }

    return newWT;
}



/*
 * Function: branch_To_Commit_WorkTree
 * ======================================
 * Permet de creer le commit (c_vide) et le WorkTree (wt_vide) correspondant a la branche (br).
 * 
 * note: Non-demande par le projet, on l'a ajoute pour se faciliter la fonction merge.
 * 
 * returns: void.
 */

void branch_To_Commit_WorkTree(char *br, Commit **c_vide, WorkTree **wt_vide){
    char path[80];

    char *hash_c = getRef(br);
    char *path_hash_c = hashToPath(hash_c);

    snprintf(path, 80, ".autosave/%s.c", path_hash_c);
    *c_vide = ftc(path);

    char *hash_wt = commitGet(*c_vide, "tree");
    char *path_hash_wt = hashToPath(hash_wt);
    snprintf(path, 80, ".autosave/%s.t", path_hash_wt);

    *wt_vide = ftwt(path);

    free(hash_c); free(path_hash_c); free(hash_wt); free(path_hash_wt);
}



/*
 * Function: merge
 * ==================
 * Fusionne la branche courante avec la branche (remote_branch) si aucun conflit n'existe. Dans ce cas :
 *      - Cree le WorkTree de fusion (avec la fonction mergeWorkTrees)
 *      - Creer le commit associe à ce nouveau WorkTree, en indiquant qui sont ses predecesseurs, et en lui ajoutant le message descriptif passe en parametre.
 *      - Realiser un enregistrement instantane du WorkTree de fusion et de ce nouveau commit
 *      - Ajouter le nouveau commit a la branche courante.
 *      - Mettre a jour la reference de la branche courante et la reference HEAD pour pointer vers ce nouveau commit.
 *      - Supprimer la reference de la branche passee en parametre.
 *      - Restaurer le projet correspondant au WorkTree de fusion.
 * 
 * returns: NULL si aucun conflit existe. 
 *          Sinon, on renvoit la liste des conflits obtenue avec la fonction merge WorkTrees.
 */

List* merge(char *remote_branch, char *message){
    char *current_branch = getCurrentBranch();
    WorkTree *wt_current, *wt_remote;
    Commit *c_current, *c_remote;

    branch_To_Commit_WorkTree(current_branch, &c_current, &wt_current);
    branch_To_Commit_WorkTree(remote_branch, &c_remote, &wt_remote);

    List *conflicts;
    WorkTree *wt_fusion = mergeWorkTrees(wt_current, wt_remote, &conflicts);

    if(conflicts == NULL){
        char *hash_tree = blobWorkTree(wt_fusion);

        Commit *c_fusion = createCommit(hash_tree);

        char *hash_wt_c = commitGet(c_current, "tree");
        char *hash_wt_r = commitGet(c_remote, "tree");

        commitSet(c_fusion, "message", message);
        commitSet(c_fusion, "predecessor", hash_wt_c);
        commitSet(c_fusion, "merged-predecessor", hash_wt_r);

        char *hash_commit = blobCommit(c_fusion);

        createUpdateRef("HEAD", hash_commit);
        createUpdateRef(current_branch, hash_commit);

        int size = strlen(remote_branch) + 10;
        char *linuxCommand = (char *)malloc(sizeof(char) * size);
        if(linuxCommand == NULL){
            printf("Erreur : allocation de linuxCommand (createDeletionCommit())\n");
            exit(1);
        }

        snprintf(linuxCommand, size, "rm .refs/%s", remote_branch);
        system(linuxCommand);

        restoreCommit(hash_commit);

        freeCommit(c_fusion);
        free(hash_tree); free(hash_wt_c); free(hash_wt_r); free(hash_commit); free(linuxCommand);
    }

    freeWorkTree(wt_current); freeWorkTree(wt_remote); freeWorkTree(wt_fusion); 
    freeCommit(c_current); freeCommit(c_remote);
    free(current_branch);

    return conflicts;
}


void createDeletionCommit(char *branch, List *conflicts, char *message){
    if(conflicts == NULL) return;

    char *current_branch = getCurrentBranch();
    myGitCheckoutBranch(branch);

    WorkTree *wt_br; Commit *commit_br;
    branch_To_Commit_WorkTree(branch, &commit_br, &wt_br);

    FILE *addFile = fopen(".add", "w");
    if(addFile == NULL){
        printf("Erreur : ouverture du .add (main - merge)\n");
        exit(1);
    }
    fclose(addFile);

    for(int i = 0; i < wt_br->n; i++){
        if(searchList(conflicts, (wt_br->tab[i])->name) == NULL){
            myGitAdd((wt_br->tab[i])->name);
        }
    }

    myGitCommit(branch, "Supression conflicts.");
    myGitCheckoutBranch(current_branch);

    freeWorkTree(wt_br);
    freeCommit(commit_br);
    free(current_branch);
}