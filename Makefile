VPATH = src build
INC = -I$(HOME)/local/include/ -I/usr/local/include/
LIB = -L$(HOME)/local/lib/ -L/usr/local/lib/
UNAME = $(shell uname)

COMPILE_OPTS = $(INC) $(LIB) -fopenmp
LINK_OPTS = -lgsl -lboost_program_options -lboost_timer -lboost_chrono -lboost_system -lgslcblas
RPATH=-Wl,-rpath=$(HOME)/local/lib:/usr/local/lib
CC = g++ -g

ifeq ($(UNAME), Darwin)
	RPATH=
endif

all : thermal.out

Stochastic.o : Stochastic.cpp Stochastic.h Reservoir.h
	$(CC) $(COMPILE_OPTS) -c src/Stochastic.cpp -o build/Stochastic.o

System.o : System.cpp System.h Utilities.h
	$(CC) $(COMPILE_OPTS) -c src/System.cpp -o build/System.o

Utilities.o : Utilities.cpp Utilities.h
	$(CC) $(COMPILE_OPTS) -c src/Utilities.cpp -o build/Utilities.o

Reservoir.o : Reservoir.cpp Reservoir.h
	$(CC) $(COMPILE_OPTS) -c src/Reservoir.cpp -o build/Reservoir.o

Ising.o : Ising.cpp Ising.h Reservoir.h
	$(CC) $(COMPILE_OPTS) -c src/Ising.cpp -o build/Ising.o

ReservoirFactory.o : ReservoirFactory.cpp ReservoirFactory.h Reservoir.h
	$(CC) $(COMPILE_OPTS) -c src/ReservoirFactory.cpp \
						-o build/ReservoirFactory.o

main.o : main.cpp
	$(CC) $(COMPILE_OPTS) -c src/main.cpp -o build/main.o

thermal.out : Stochastic.o System.o Utilities.o main.o Ising.o Reservoir.o\
		ReservoirFactory.o
	$(CC) $(COMPILE_OPTS) $(LINK_OPTS) build/System.o build/Stochastic.o build/Utilities.o build/main.o build/Ising.o build/Reservoir.o build/ReservoirFactory.o -o thermal.out $(RPATH)

.PHONY: clean run

clean :
	rm -f build/*.o thermal.out *~ src/*~

run : thermal.out
	./thermal.out
