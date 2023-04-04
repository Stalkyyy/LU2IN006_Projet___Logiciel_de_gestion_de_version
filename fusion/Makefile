CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_WORK = fusion.o fusionTest.o ../branch/branch.o ../commit/commit.o ../work_FileTree/work_FileTree.o  ../sha256/sha256.o ../cellList/cellList.o

all : fusion_test

fusion_test : $(OBJFILES_WORK)
	$(CC) $(CFLAGS) -o fusion_test $(OBJFILES_WORK)

clean : 
	rm -f $(OBJFILES_WORK) .current_branch 
	rm -r .autosave .tmp .refs