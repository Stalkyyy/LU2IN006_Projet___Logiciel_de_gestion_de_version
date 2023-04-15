CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_WORK = work_FileTree.o work_FileTreeTest.o ../sha256/sha256.o ../cellList/cellList.o

all : work_FileTree_test

work_FileTree_test : $(OBJFILES_WORK)
	$(CC) $(CFLAGS) -o work_FileTree_test $(OBJFILES_WORK)

clean : 
	rm -f $(OBJFILES_WORK) work_FileTree_test test_wttf.txt
	rm -r autosave tmp