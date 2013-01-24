VPATH = thermaleraser:GSL/libs/
INC = -I $(HOME)/local/include
LIB = -L $(HOME)/local/lib/

COMPILE_OPTS = $(INC) $(LIB) -fopenmp
LINK_OPTS = -lgsl

all : thermal.out

States.o : States.cpp States.h
	g++ $(COMPILE_OPTS) -c thermaleraser/States.cpp

System.o : System.cpp System.h Utilities.h
	g++ $(COMPILE_OPTS) -c thermaleraser/System.cpp

Utilities.o : Utilities.cpp Utilities.h
	g++ $(COMPILE_OPTS) -c thermaleraser/Utilities.cpp

main.o : main.cpp
	g++ $(COMPILE_OPTS) -c thermaleraser/main.cpp

thermal.out : States.o System.o Utilities.o main.o
	g++ $(COMPILE_OPTS) $(LINK_OPTS) System.o States.o Utilities.o main.o -o thermal.out

.PHONY: clean

clean :
	rm *.o thermal.out