#ifndef _FUSION_H_

WorkTree* mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts);
void branch_To_Commit_WorkTree(char *br, Commit **c_vide, WorkTree **wt_vide);
List* merge(char *remote_branch, char *message);
void createDeletionCommit(char *branch, List *conflicts, char *message);

#endif