#ifndef _BRANCH_H_

void initBranch();
int branchExists(char *branch);
void createBranch(char *branch);
char* getCurrentBranch();
void printBranch(char *branch);
List* branchList(char *branch);
List* getAllCommits();

void restoreCommit(char *hash_commit);
void myGitCheckoutBranch(char *branch);
List *filterList(List *l, char *pattern);
void myGitCheckoutCommit(char *pattern);
void createDeletionCommit(char *branch, List *conflicts, char *message);



#endif