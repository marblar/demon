#include "Ising.h"
#include <math.h>
#include <assert.h>

#define SQUARED(x) x*x

namespace ising {
    inline int s(int x,int y,int z) {
        return x<<2 + y<<1 + z;
    }
}

using namespace ising;

int IsingReservoir::interactWithBit(int bit) {
    if (currentState->bit != bit) {
        currentState=currentState->bitFlipState;
    }
    
    assert(ceil(constants.tau)>0 &&
           "Figure out how tau converts to discrete time.");
    int iterations = ceil(constants.tau);
    
    for (int k = 0; k<iterations; k++) {
        parity = 0;
        this->isingStep();
        this->wheelStep();
    }
    
    return currentState->bit;
}

inline int IsingReservoir::isingStep() {
    Coordinate coord;
    int row = 0;
    int column = (currentStepType == odd ? row + 1 : row) % 2;
    while (row<=isingSide) {
        while (column<=isingSide) {
            
            //Evolve each cell.
            char &cell = getCell(coord);
            Coordinate coord(row,column);
            if (countHighNeighbors(coord) == 2) {
                cell = (cell + 1) % 2;
            }
            parity += cell;
            
            column+=2;
        }
        row++;
        column = (currentStepType == odd ? row + 1 : row) % 2;
    }
}

inline void IsingReservoir::wheelStep() {
    char &s1 = getCell(interactionCells.first);
    char &s2 = getCell(interactionCells.second);
    
    TransitionTable currentTable = transitions[currentState];
    SystemState *nextState = currentTable[s(s1,s2,parity)];
    
    currentState = nextState;
}

inline int IsingReservoir::countHighNeighbors(Coordinate c) {
    CoordinateList neighbors = getNeighbors(c);
    return countHigh(neighbors);
}

void IsingReservoir::initializeCellsWithRNG(gsl_rng *RNG, int N) {
    for (int k=0; k<SQUARED(this->isingSide); k++) {
        int row = (int)(k % this->isingSide);
        int column = (int)(k / this->isingSide);
        
        //Randomize the cells.
        this->cells[column][row] = gsl_rng_uniform_int(RNG,2);
    }
    
    // The metropolis algorithm!
    for (int k=0; k<N; k++) {
        Coordinate coord;
        coord.x = gsl_rng_uniform_int(RNG,isingSide);
        coord.y = gsl_rng_uniform_int(RNG,isingSide);
        
        char &cell = getCell(coord);
        
        int dE = 0;
        if (cell == 0) {
            dE = 4 - 2 * countHighNeighbors(coord);
        } else {
            //dE = 4 - 2 * (4 - countHigh(neighbors));
            //dE = 4 - 8 + 2 * countHigh(neighbors);
            dE = 2 * countHighNeighbors(coord) - 4;
        }
        
        // TODO: Memoize me.
        if ( exp(constants.beta() * dE) < gsl_rng_uniform(RNG) ) {
            char &cell = getCell(coord);
            cell = (cell + 1) % 2;
        }
    }
}

inline char &IsingReservoir::getCell(const Coordinate c) {
    return cells[c.x][c.y];
}

inline int IsingReservoir::countHigh(CoordinateList list) {
    int highCount = 0;
    for (CoordIterator it = list.begin(); it!=list.end(); ++it) {
        if (getCell(*it) == 1) {
            ++highCount;
        }
    }
    return highCount;
}

inline int boundsCheck(int x, const int max) {
    if (x < 0) {
        return max + x;
    }
    return x % max;
}

IsingReservoir::CoordinateList IsingReservoir::getNeighbors(const Coordinate c){
    std::vector<Coordinate> neighbors;
    for (int k = 0; k<4; k++) {
        Coordinate neighbor;
        switch (k) {
            case 0: // North
                neighbor.x = c.x;
                neighbor.y = c.y - 1;
                break;
            
            case 1: // South
                neighbor.x = c.x;
                neighbor.y = c.y + 1;
                break;
                
            case 2: // East
                neighbor.x = c.x - 1;
                neighbor.y = c.y;
                break;
                
            case 3: // West
                neighbor.x = c.x + 1;
                neighbor.y = c.y;
                break;
                
            default: // We should never be here.
                assert(0 && "Unreachable.");
                break;
        }
        neighbor.x = boundsCheck(neighbor.x, isingSide);
        neighbor.y = boundsCheck(neighbor.y, isingSide);
        neighbors.push_back(neighbor);
    }
    return neighbors;
}
            
IsingReservoir::~IsingReservoir() {
    for (int k=0; k<this->isingSide; k++) {
        delete [] cells[k];
    }
    delete [] cells;
}

IsingReservoir::IsingReservoir(gsl_rng *RNG, Constants constants, int IS) : Reservoir(constants), isingSide(IS) {
    setupStateTable();
    this->cells = new char *[IS];
    for (int k=0; k<IS; k++) {
        this->cells[k]=new char[IS];
    }
    this->initializeCellsWithRNG(RNG);
}

inline void IsingReservoir::setupStateTable() {
    //TODO: Custom states. This is goofy.
    //Source: http://imgur.com/FF5TBQh
    
    #define init(XX,StateXX) \
                TransitionTable XX;\
                for (int k=0; k<8; k++) \
                    XX[k]=&StateXX;
    init(A1,StateA1);
    A1[s(0,0,1)] = A1[s(1,0,1)] = &StateB1;
    transitions[&StateA1]=A1;
    
    init(B1,StateB1);
    B1[s(0,0,1)] = B1[s(1,0,1)] = &StateA1;
    B1[s(1,1,1)] = B1[s(0,1,1)] = &StateC1;
    transitions[&StateB1]=B1;
    
    init(C1,StateC1);
    C1[s(0,1,1)] = C1[s(1,1,1)] = &StateB1;
    C1[s(0,0,1)] = C1[s(0,0,0)] = &StateA0;
    transitions[&StateC1]=C1;
    
    init(A0,StateA0);
    A0[s(0,1,0)] = A0[s(0,1,1)] = &StateC1;
    A0[s(1,0,1)] = A0[s(0,0,1)] = &StateB0;
    transitions[&StateA0]=A0;
    
    init(B0,StateB0);
    B0[s(0,0,1)] = B0[s(1,0,1)] = &StateB0;
    B0[s(1,1,1)] = B0[s(0,1,1)] = &StateA0;
    
    init(C0,StateC0);
    B0[s(1,1,1)] = B0[s(0,1,1)] = &StateB0;
    
    //TODO: TEST ME.
}

Reservoir *IsingReservoir::IsingFactory::create(gsl_rng *RNG, Constants constants) {
    return new IsingReservoir(RNG,constants,dimension);
}
