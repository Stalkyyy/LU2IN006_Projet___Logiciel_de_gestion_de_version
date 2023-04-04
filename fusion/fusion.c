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

//==================================================================================================

/*
 * Exercice 11 - Une première méthode de fusion. (11.1 - 11.2)
 */

//==================================================================================================



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


void branch_To_Commit_WorkTree(char *br, Commit **c_vide, WorkTree **wt_vide){
    char path[80];

    char *hash_c = getRef(br);
    char *path_hash_c = hashToPath(hash_c);
    strcpy(path, ".autosave/");
    strcat(path, path_hash_c);
    strcat(path, ".c");
    *c_vide = ftc(path);

    char *hash_wt = commitGet(*c_vide, "tree");
    char *path_hash_wt = hashToPath(hash_wt);
    strcpy(path, ".autosave/");
    strcat(path, path_hash_wt);
    strcat(path, ".t");
    *wt_vide = ftwt(path);

    free(hash_c); free(path_hash_c); free(hash_wt); free(path_hash_wt);
}


List* merge(char *remote_branch, char *message){
    char *current_branch = getCurrentBranch();
    WorkTree *wt_current, *wt_remote;
    Commit *c_current, *c_remote;

    branch_To_Commit_WorkTree(current_branch, &c_current, &wt_current);
    branch_To_Commit_WorkTree(remote_branch, &c_remote, &wt_remote);

    List *conflicts;
    //WorkTree *wt_fusion = mergeWorkTrees(wt_current, wt_remote, &conflicts);
    return conflicts;
}