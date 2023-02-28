CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_MAIN = main.o sha256.o cellList.o 
OBJFILES_SHA256 = sha256.o sha256Test.o 
OBJFILES_CELLIST = cellList.o cellListTest.o


all : main_test

main_test : $(OBJFILES_MAIN)
	$(CC) $(CFLAGS) -o main_test $(OBJFILES_MAIN)

sha256 : $(OBJFILES_SHA256)
	$(CC) $(CFLAGS) -o sha256_test $(OBJFILES_SHA256)

cellList : $(OBJFILES_CELLIST)
	$(CC) $(CFLAGS) -o cellList_test $(OBJFILES_CELLIST)

clean : 
	rm -f $(OBJFILES_MAIN) $(OBJFILES_SHA256) $(OBJFILES_CELLIST) main_test sha256_test cellList_test hash_sha256Test.txt