#include "Ising.h"
#include <math.h>
#include <assert.h>
#include <stack>

#define SQUARED(x) x*x

namespace ising {
    inline int s(char x,char y,char z) {
        return (x<<2) + (y<<1) + (z);
    }
    
    inline int boundsCheck(int x, const int max) {
        if (x < 0) {
            return max + x;
        }
        if (x >= max) {
            return x-max;
        }
        return x;
    }
}

using namespace ising;

Reservoir::InteractionResult IsingReservoir::interactWithBit(int bit) {
    InteractionResult result;
    if (currentState->bit != bit) {
        currentState=currentState->bitFlipState;
    }
    
    assert(ceil(constants.tau)>0 &&
           "Figure out how tau converts to discrete time.");
    int iterations = ceil(constants.tau);
    
    for (int k = 0; k<iterations; k++) {
        parity = 0;
        this->isingStep(result);
        this->wheelStep(result);
    }
    
    result.bit = currentState->bit;
    
    return result;
}

inline void IsingReservoir::isingStep(InteractionResult &result) {
    int row = 0;
    int column = (currentStepType == odd ? row + 1 : row) % 2;
    while (row<isingSide) {
        while (column<isingSide) {
            Coordinate coord(row,column);
            //Evolve each cell.
            char &cell = getCell(coord);
            if (countHighNeighbors(coord) == 2) {
                cell = (cell + 1) % 2;
            }
            parity += cell;
            
            column+=2;
        }
        row++;
        column = (currentStepType == odd ? row + 1 : row) % 2;
    }
    currentStepType = (currentStepType == odd) ? even : odd;
}

void IsingReservoir::reset() {
    currentState = randomState();
    for (int k = 0; k<20; k++) {
        clusterMethod();
    }
}

void IsingReservoir::clusterMethod() {

//  http://link.springer.com/chapter/10.1007%2F3-540-35273-2_1?LI=true#page-1

    std::stack<Coordinate> workStack;
    Coordinate currentCoord(rand()%isingSide,rand()%isingSide);
    workStack.push(currentCoord);
    
    //Basic BFT
    while (!workStack.empty()) {
        currentCoord = workStack.top();
        char &currentCell = getCell(currentCoord);
        workStack.pop();
        Neighbors neighbors = getNeighbors(currentCoord);
        for (int k=0; k<4; k++) {
            Coordinate neighborCoord = neighbors.coordinates[k];
            char &neighborCell = getCell(neighborCoord);
            
            if (currentCell!=neighborCell) {
                continue;
            }
            
            double inclusionProbability = 1 - exp(-2*constants.beta());
            if (gsl_rng_get(RNG)<inclusionProbability) {
                workStack.push(neighborCoord);
            }
        }
        
        // Once the current cell has been flipped, we don't need
        // to worry about keeping track of considered bonds.
        currentCell = (currentCell + 1) % 2;
    }
    
}

inline void IsingReservoir::wheelStep(InteractionResult &result) {
    char &s1 = getCell(interactionCells.first);
    char &s2 = getCell(interactionCells.second);
    
    int inputState = s(s1,s2,parity % 2);
    
    SystemState *lastState = currentState;
    TransitionTable currentTable = transitions[currentState];
    SystemState *nextState = currentTable[inputState];
    
    assert(currentTable.size());
    assert(nextState);
    
    const int &oldBit = nextState->bit;
    const int &newBit = currentState->bit;
    
    //The abstraction is leaking, but this saves a very complicated table.
    if (oldBit != newBit) {
        int initialEnergy = getEnergy(interactionCells.first) \
                                + getEnergy(interactionCells.second) - s1 ^ s2;
        
        if (newBit>oldBit) {
            s1 = 0;
            s2 = 0;
        } else if (newBit < oldBit) {
            s1 = 0;
            s2 = 1;
        }
        
        int finalEnergy = getEnergy(interactionCells.first) \
                                + getEnergy(interactionCells.second) - s1 ^ s2;
        
        result.work += initialEnergy - finalEnergy;
    }
    
    currentState = nextState;
}

inline int IsingReservoir::getEnergy(Coordinate c) {
    int highNeighbors = countHighNeighbors(c);
    
    if (getCell(c) == 1) {
        return 4 - highNeighbors;
    } else {
        return highNeighbors;
    }
}

inline int IsingReservoir::countHighNeighbors(Coordinate c) {
    Neighbors neighbors = getNeighbors(c);
    return countHigh(neighbors);
}

void IsingReservoir::initializeCellsWithRNG(gsl_rng *RNG, int N) {
    reset();
}

inline char &IsingReservoir::getCell(const Coordinate c) {
    //FIXME: Bounds check?
    return cells[c.y][c.x];
}

inline int IsingReservoir::countHigh(Neighbors list) {
    int highCount = 0;
    for (int k=0; k<4; k++) {
        highCount+=getCell(list.coordinates[k]);
    }
    return highCount;
}


IsingReservoir::Neighbors IsingReservoir::getNeighbors(const Coordinate c){
    Neighbors neighbors;
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
        neighbors.coordinates[k] = neighbor;
    }
    return neighbors;
}
            
IsingReservoir::~IsingReservoir() {
    for (int k=0; k<this->isingSide; k++) {
        delete [] cells[k];
    }
    delete [] cells;
}

IsingReservoir::IsingReservoir(gsl_rng *RNG_, Constants constants, int IS) : Reservoir(constants), isingSide(IS), RNG(RNG_) {
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
                for (char k=0; k<8; k++) \
                    XX[k]=&StateXX;
    init(A1,StateA1);
    A1[s(0,0,1)] = &StateB1;
    A1[s(1,0,1)] = &StateB1;
    transitions[&StateA1]=A1;
    
    init(B1,StateB1);
    B1[s(0,0,1)] = &StateA1;
    B1[s(1,0,1)] = &StateA1;
    B1[s(1,1,1)] = &StateC1;
    B1[s(0,1,1)] = &StateC1;
    transitions[&StateB1]=B1;
    
    init(C1,StateC1);
    C1[s(0,1,1)] = &StateB1;
    C1[s(1,1,1)] = &StateB1;
    C1[s(0,0,1)] = &StateA0;
    C1[s(0,0,0)] = &StateA0;
    transitions[&StateC1]=C1;
    
    init(A0,StateA0);
    A0[s(0,1,0)] = &StateC1;
    A0[s(0,1,1)] = &StateC1;
    A0[s(1,0,1)] = &StateB0;
    A0[s(0,0,1)] = &StateB0;
    transitions[&StateA0]=A0;
    
    init(B0,StateB0);
    B0[s(0,0,1)] = &StateB0;
    B0[s(1,0,1)] = &StateB0;
    B0[s(1,1,1)] = &StateA0;
    B0[s(0,1,1)] = &StateA0;
    transitions[&StateB0] = B0;
    
    init(C0,StateC0);
    C0[s(1,1,1)] = &StateB0;
    C0[s(0,1,1)] = &StateB0;
    transitions[&StateC0] = C0;
    //TODO: TEST ME.
}

Reservoir *IsingReservoir::IsingFactory::create(gsl_rng *RNG, Constants constants) {
    return new IsingReservoir(RNG,constants,dimension);
}
