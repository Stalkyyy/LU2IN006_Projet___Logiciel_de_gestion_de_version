CC = gcc
CFLAGS = -Wall -ggdb
OBJFILES = main.o sha256.o

all : main_test

main_test : $(OBJFILES)
	$(CC) $(CFLAGS) -o main_test $(OBJFILES)

clean : 
	rm -f $(OBJFILES) main_test