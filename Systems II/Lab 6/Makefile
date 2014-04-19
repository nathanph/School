#
# Student makefile for Cache Lab
# 
CC = gcc
CFLAGS = -g -Wall -Werror 

all: resolve

resolve: resolve.c resolve.h SymbolList.c SymbolList.h
	$(CC) $(CFLAGS) -o resolve resolve.c SymbolList.c -lm 

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f resolve
