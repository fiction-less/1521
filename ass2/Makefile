# COMP1521 22T2 --- Assignment 2: `Bytelocker'
# YOU DO NOT NEED TO MODIFY THIS FILE

ifneq (, $(shell which dcc))
CC	= dcc
else
CC	= clang
endif

CFLAGS	= -Wall 

.PHONY:	all
all:	bytelocker

bytelocker:		bytelocker.c bytelocker.h main.c
	$(CC) $(CFLAGS) main.c bytelocker.c -o bytelocker

.PHONY: clean
clean:
	-rm -f bytelocker

.PHONY: test give

test: bytelocker.c
	1521 autotest bytelocker bytelocker.c

give: bytelocker.c
	give cs1521 ass2_bytelocker bytelocker.c
