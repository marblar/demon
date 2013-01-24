
VPATH = src
INC = -I$(HOME)/local/include -I/usr/local/include
LIB = -L$(HOME)/local/lib/ -L/usr/local/lib

COMPILE_OPTS = $(INC) $(LIB) -fopenmp
LINK_OPTS = -lgsl -lboost_program_options
CC = g++-4.2

all : thermal.out

States.o : States.cpp States.h
	$(CC) $(COMPILE_OPTS) -c src/States.cpp

System.o : System.cpp System.h Utilities.h
	$(CC) $(COMPILE_OPTS) -c src/System.cpp

Utilities.o : Utilities.cpp Utilities.h
	$(CC) $(COMPILE_OPTS) -c src/Utilities.cpp

main.o : main.cpp
	$(CC) $(COMPILE_OPTS) -c src/main.cpp

thermal.out : States.o System.o Utilities.o main.o
	$(CC) $(COMPILE_OPTS) $(LINK_OPTS) System.o States.o Utilities.o main.o -o thermal.out

.PHONY: clean run

clean :
	rm -f *.o thermal.out *~

run : thermal.out
	./thermal.out