CC = gcc
CFLAGS = -Wall -ggdb

OBJFILES_SHA256 = sha256.o sha256Test.o 

all : sha256_test

sha256_test : $(OBJFILES_SHA256)
	$(CC) $(CFLAGS) -o sha256_test $(OBJFILES_SHA256)

clean :
	rm -f $(OBJFILES_SHA256) sha256_test