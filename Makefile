CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_MYGIT = myGit.o ./sha256/sha256.o ./cellList/cellList.o ./work_FileTree/work_FileTree.o ./commit/commit.o ./branch/branch.o

all : myGit_test

myGit_test : $(OBJFILES_MYGIT)
	$(CC) $(CFLAGS) -o myGit_test $(OBJFILES_MYGIT)

clean : 
	rm -f $(OBJFILES_MYGIT) $(OBJFILES_SHA256) $(OBJFILES_CELLIST) myGit_test .add .current_branch
	rm -r .refs tmp autosave