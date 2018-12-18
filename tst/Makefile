CFLAG = -g -c -std=c++17 -fmax-errors=1
INCLUDE = -I../ 
CC = g++
OBJ = $(patsubst %.cc, ../obj/%.tst.o, $(wildcard *.cc))

all : ../obj/catch.tx $(OBJ)

../obj/%.tst.o : %.cc
	$(CC) $(INCLUDE) $(CFLAG) $< -o $@

../obj/catch.tx : catch.cpp
	$(CC) $(INCLUDE) $(CFLAG) $< -o $@
