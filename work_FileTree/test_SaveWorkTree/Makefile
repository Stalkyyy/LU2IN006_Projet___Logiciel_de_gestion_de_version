CC = gcc
CFLAGS = -Wall -ggdb
OBJFILES = main.o biblioLC.o entreeSortieLC.o

all : main_test

main_test : $(OBJFILES)
	$(CC) $(CFLAGS) -o main_test $(OBJFILES)

clean :
	rm -f $(OBJFILES) *.o main_test