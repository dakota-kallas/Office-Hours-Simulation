#
# A simple makefile
#

CC=gcc
CFLAGS=-g -O0 -Wall

PROGS=simulator

simulator: simulator.c stu_list.o
	$(CC) simulator.c stu_list.o -o simulator $(CFLAGS) -UFINAL

simulator-final: simulator.c stu_list.o
	$(CC) simulator.c stu_list.o -o simulator $(CFLAGS) -DFINAL

stu_list.o: stu_list.c stu_list.h
	$(CC) stu_list.c -c -o stu_list.o $(CFLAGS)

clean:
	rm -f $(PROGS)
	rm -rf *.o *.dSYM *~
