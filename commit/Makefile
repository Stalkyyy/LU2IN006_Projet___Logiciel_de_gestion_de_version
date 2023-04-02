CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_WORK = commit.o commitTest.o ../work_FileTree/work_FileTree.o  ../sha256/sha256.o ../cellList/cellList.o

all : commit_test

commit_test : $(OBJFILES_WORK)
	$(CC) $(CFLAGS) -o commit_test $(OBJFILES_WORK)

clean : 
	rm -f $(OBJFILES_WORK) commit_test test_ctf.txt
	rm -r autosave tmp .refs