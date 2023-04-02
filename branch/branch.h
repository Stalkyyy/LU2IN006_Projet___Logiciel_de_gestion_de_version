#ifndef _BRANCH_H_
//#include "../cellList/cellList.h"

void initBranch();
int branchExists(char *branch);
void createBranch(char *branch);
char* getCurrentBranch();
void printBranch(char *branch);
List* branchList(char *branch);
List* getAllCommits();



#endif