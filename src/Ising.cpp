#include "Ising.h"
#include "Utilities.h"

#include <math.h>
#include <assert.h>
#include <stack>

#define SQUARED(x) x*x

namespace ising {
    int s(char x,char y,char z) {
        return (x<<2) + (y<<1) + (z);
    }
    
    int boundsCheck(int x, const int max) {
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
    
    assert(ceil(constants.getTau())>0 &&
           "Figure out how tau converts to discrete time.");
    int iterations = ceil(constants.getTau());
    
    for (int k = 0; k<iterations; k++) {
        parity = 0;
        this->isingStep(result);
        this->wheelStep(result);
    }
    
    result.bit = currentState->bit;
    
    return result;
}

void IsingReservoir::isingStep(InteractionResult &result) {
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
    for (int k = 0; k<clusters; k++) {
        clusterMethod();
    }
}

void IsingReservoir::clusterMethod() {

//  http://link.springer.com/chapter/10.1007%2F3-540-35273-2_1?LI=true#page-1

    std::stack<Coordinate> workStack;
    Coordinate currentCoord(rand()%isingSide,rand()%isingSide);
    workStack.push(currentCoord);
    char &currentCell = getCell(currentCoord);
    unsigned char clusterBit = currentCell;
    currentCell^=1;

    
    double inclusionProbability = 1 - exp(-2*constants.getBeta());
    assert(inclusionProbability<=1 && inclusionProbability>=0);
    
    //Basic BFT
    while (!workStack.empty()) {
        currentCoord = workStack.top();
        char &currentCell = getCell(currentCoord);
        workStack.pop();
        Neighbors neighbors = getNeighbors(currentCoord);
        for (int k=0; k<4; k++) {
            Coordinate neighborCoord = neighbors.coordinates[k];
            char &neighborCell = getCell(neighborCoord);
            
            if (neighborCell == clusterBit &&
                    gsl_rng_uniform(RNG)<inclusionProbability) {
                workStack.push(neighborCoord);
                neighborCell^=1;
            }
        }
        assert(workStack.size()<SQUARED(isingSide));
    }
    
}

void IsingReservoir::wheelStep(InteractionResult &result) {
    char &s1 = getCell(interactionCells.first);
    char &s2 = getCell(interactionCells.second);
    
    int inputState = s(s1,s2,parity % 2);
    
    SystemState *lastState = currentState;
    
    CheckTransitionRuleError(transitions.count(currentState),
                             EmptyTransitionRuleError);
    
    TransitionTable currentTable = transitions[currentState];
    SystemState *nextState = currentTable[inputState];
    
    CheckTransitionRuleError(inputState<8 && inputState>=0 &&
                                "Invalid input state.",
                             InvalidInputStateError);
    CheckTransitionRuleError(currentTable.size() == 8 &&
                                "Transition table size should be 8",
                             InvalidTableSizeError);
    CheckTransitionRuleError(nextState && "nextState must not be null.",
                             TransitionDeadEndError);
    
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

int IsingReservoir::getEnergy(Coordinate c) {
    int highNeighbors = countHighNeighbors(c);
    
    if (getCell(c) == 1) {
        return 4 - highNeighbors;
    } else {
        return highNeighbors;
    }
}

int IsingReservoir::countHighNeighbors(Coordinate c) {
    Neighbors neighbors = getNeighbors(c);
    return countHigh(neighbors);
}

void IsingReservoir::initializeCellsWithRNG(gsl_rng *RNG, int N) {
    reset();
}

char &IsingReservoir::getCell(const Coordinate c) {
    //FIXME: Bounds check?
    return cells[c.y][c.x];
}

int IsingReservoir::countHigh(Neighbors list) {
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

IsingReservoir::IsingReservoir(gsl_rng *RNG_, Constants constants,
                               int IS, int cls, TransitionRule rule) :
        Reservoir(constants), isingSide(IS), RNG(RNG_), clusters(cls) {
    transitions = rule;
    cells = new char *[IS];
    for (int k=0; k<IS; k++) {
        cells[k]=new char[IS];
        
        std::fill(cells[k], cells[k]+IS, 0);
    }
    this->initializeCellsWithRNG(RNG);
    currentState = randomState();
}


TransitionRule defaultTransitionRule() {
    //Source: http://imgur.com/FF5TBQh
    
    TransitionRule transitions;
    
    TTable(A1);
    A1[s(0,0,1)] = &StateB1;
    A1[s(1,0,1)] = &StateB1;
    transitions[&StateA1]=A1;
    
    TTable(B1);
    B1[s(0,0,1)] = &StateA1;
    B1[s(1,0,1)] = &StateA1;
    B1[s(1,1,1)] = &StateC1;
    B1[s(0,1,1)] = &StateC1;
    transitions[&StateB1]=B1;
    
    TTable(C1);
    C1[s(0,1,1)] = &StateB1;
    C1[s(1,1,1)] = &StateB1;
    C1[s(0,0,1)] = &StateA0;
    C1[s(0,0,0)] = &StateA0;
    transitions[&StateC1]=C1;
    
    TTable(A0);
    A0[s(0,1,0)] = &StateC1;
    A0[s(0,1,1)] = &StateC1;
    A0[s(1,0,1)] = &StateB0;
    A0[s(0,0,1)] = &StateB0;
    transitions[&StateA0]=A0;
    
    TTable(B0);
    B0[s(0,0,1)] = &StateB0;
    B0[s(1,0,1)] = &StateB0;
    B0[s(1,1,1)] = &StateA0;
    B0[s(0,1,1)] = &StateA0;
    transitions[&StateB0] = B0;
    
    TTable(C0);
    C0[s(1,1,1)] = &StateB0;
    C0[s(0,1,1)] = &StateB0;
    transitions[&StateC0] = C0;
    
    return transitions;
}

Reservoir *IsingReservoir::IsingFactory::create(gsl_rng *RNG, Constants constants) {
    return new IsingReservoir(RNG,constants,dimension,clusters);
}

void isingEnergyDistribution(int d, int clusters) {
    double delta = .5;
    double tau = 1;
    double epsilon = 0;
    Constants constants(delta,epsilon,tau);
    
    gsl_rng *RNG = GSLRandomNumberGenerator();
    for (int energy = 0; energy!=4; energy++) {
        constants.setEpsilon(.15*energy+.25);
        printf("Beta: %lf\n",constants.getBeta());
        IsingReservoir *reservoir = new IsingReservoir(RNG,constants,d,clusters);
        reservoir->reset();
        for (int k=0; k<100; k++) {
            printf("%d\n",reservoir->totalEnergy());
            reservoir->reset();
        }
        delete reservoir;
        printf("\n");
    }
}

int IsingReservoir::totalEnergy() {
    int N = SQUARED(isingSide);
    int energy = 0;
    for (int k = 0; k<N; k++) {
        int row = (int)(k % this->isingSide);
        int column = (int)(k / this->isingSide);
        energy += getEnergy(Coordinate(row, column));
    }
    // This method overcounts by two, considering both the forward and
    // backword bonds.
    return energy/2;
}
