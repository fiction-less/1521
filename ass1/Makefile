# COMP1521 22T2 --- Assignment 1: `Connect Four' 
# YOU DO NOT NEED TO MODIFY THIS FILE

ifneq (, $(shell which dcc))
CC	= dcc
else
CC	= clang
endif

CFLAGS	= -Wall -Werror

.PHONY:	all
all:	connect_four

connect_four: connect_four.c
	$(CC) $(CFLAGS) connect_four.c -o connect_four

.PHONY: clean
clean:
	-rm -f connect_four

.PHONY: test give

test: connect_four.s
	1521 autotest connect_four connect_four.s

give: connect_four.s
	give cs1521 ass1_connect_four connect_four.s
