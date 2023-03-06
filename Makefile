CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_MAIN = main.o ./sha256/sha256.o ./cellList/cellList.o

all : main_test

main_test : $(OBJFILES_MAIN)
	$(CC) $(CFLAGS) -o main_test $(OBJFILES_MAIN)

clean : 
	rm -f $(OBJFILES_MAIN) $(OBJFILES_SHA256) $(OBJFILES_CELLIST) main_test sha256_test cellList_test hash_sha256Test.txt