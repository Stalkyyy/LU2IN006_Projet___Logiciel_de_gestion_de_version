CC = gcc
CFLAGS = -Wall -ggdb
OBJFILES = biblioH.o entreeSortieH.o main.o

all : main_test

main_test : $(OBJFILES)
	$(CC) $(CFLAGS) -o main_test $(OBJFILES) -lm

clean :
	rm -f $(OBJFILES) *.o main_test