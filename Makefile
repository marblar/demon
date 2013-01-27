#A change for tate

VPATH = src
INC = -I$(HOME)/local/include -I/usr/local/include
LIB = -L$(HOME)/local/lib/ -L/usr/local/lib

COMPILE_OPTS = $(INC) $(LIB) -O3 -fopenmp
LINK_OPTS = -lgsl -lboost_program_options -lboost_timer -lboost_chrono -lboost_system -lgslcblas
RPATH=-Wl,-rpath=$(HOME)/local/lib:/usr/local/lib
CC = g++ -g

all : thermal.out

Stochastic.o : Stochastic.cpp Stochastic.h
	$(CC) $(COMPILE_OPTS) -c src/Stochastic.cpp

System.o : System.cpp System.h Utilities.h
	$(CC) $(COMPILE_OPTS) -c src/System.cpp

Utilities.o : Utilities.cpp Utilities.h
	$(CC) $(COMPILE_OPTS) -c src/Utilities.cpp

main.o : main.cpp
	$(CC) $(COMPILE_OPTS) -c src/main.cpp

thermal.out : Stochastic.o System.o Utilities.o main.o
	$(CC) $(COMPILE_OPTS) $(LINK_OPTS) System.o Stochastic.o Utilities.o main.o -o thermal.out $(RPATH)

.PHONY: clean run

clean :
	rm -f *.o thermal.out *~

run : thermal.out
	./thermal.out