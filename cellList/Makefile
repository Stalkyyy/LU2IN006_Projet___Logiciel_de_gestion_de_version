CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_CELLIST = cellList.o cellListTest.o ../sha256/sha256.o

all : cellList_test

cellList_test : $(OBJFILES_CELLIST)
	$(CC) $(CFLAGS) -o cellList_test $(OBJFILES_CELLIST)

clean :
	rm -f CLEAN_CELLIST = $(OBJFILES_CELLIST) cellList_test test_ltof.txt copie.txt test_listdir.txt
	rm -r .autosave .tmp