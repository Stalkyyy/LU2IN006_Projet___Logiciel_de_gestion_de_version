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


int main(){
    WorkTree *wt1 = initWorkTree();
    WorkTree *wt2 = initWorkTree();

    appendWorkTree(wt1, "Test1-OUI", NULL, 0);
    appendWorkTree(wt1, "Test2-OUI", NULL, 0);
    appendWorkTree(wt1, "Test3-NON", NULL, 0);

    appendWorkTree(wt2, "Test4-NON", NULL, 0);
    appendWorkTree(wt2, "Test2-OUI", NULL, 0);
    appendWorkTree(wt2, "Test5-NON", NULL, 0);
    appendWorkTree(wt2, "Test1-OUI", NULL, 0);

    List *conflicts = NULL;

    WorkTree *wt_no_conflicts = mergeWorkTrees(wt1, wt2, &conflicts);

    Cell *c = *conflicts;
    printf("Liste des conflits :\n");
    while(c){
        printf("   -> %s\n", c->data);
        c = c->next;
    }

    printf("\nWorkTree du non conflit :\n");
    for(int i = 0; i < wt_no_conflicts->n; i++){
        printf("   -> %s\n", wt_no_conflicts->tab[i]->name);
    }

    printf("\n");
    /*
     * A faire avant de le lancer
     * Compiler MyGit, créer une branche master_Change avec un commit différent de master grâce au fichier test.txt.
     * 
     * ../myGit init
     * ../myGit add test.txt fusion.h
     * ../myGit commit master -m PremierCommitMaster
     * ../myGit branch master_Change
     * ../myGit add test.txt Makefile
     * ../myGit commit master_Change -m PremierCommitMaster_Change
     */

    WorkTree *wt_master_2 = NULL; Commit *c_master_2 = NULL;
    WorkTree *wt_master = NULL; Commit *c_master = NULL;

    branch_To_Commit_WorkTree("master", &c_master, &wt_master);
    branch_To_Commit_WorkTree("master_Change", &c_master_2, &wt_master_2);

    printf("Elements de master :\n");
    for(int i = 0; i < wt_master->n; i++){
        printf("   -> %s\n", wt_master->tab[i]->name);
    }

    printf("\nElements de master_Change :\n");
    for(int i = 0; i < wt_master_2->n; i++){
        printf("   -> %s\n", wt_master_2->tab[i]->name);
    }




    freeList(conflicts);
    freeWorkTree(wt1), freeWorkTree(wt2), freeWorkTree(wt_no_conflicts);
    freeWorkTree(wt_master); freeWorkTree(wt_master_2);
    freeCommit(c_master); freeCommit(c_master_2);
    return 0;
}