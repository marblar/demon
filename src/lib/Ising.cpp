#include "Ising.h"
#include "Utilities.h"

#include <math.h>
#include <assert.h>
#include <boost/foreach.hpp>

#define SQUARED(x) x*x
using namespace DemonBase;
using namespace Ising;

int Ising::s(char x,char y,char z) {
    return (x<<2) + (y<<1) + (z & 1);
}

DemonBase::Reservoir::InteractionResult Ising::Reservoir::interactWithBit(int bit) {
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

void Ising::Reservoir::isingStep(InteractionResult &result) {
    Grid::subset::const_iterator it = currentStepType ? \
        grid.odds.begin() : grid.evens.begin();
    Grid::subset::const_iterator end = currentStepType ? \
        grid.odds.end() : grid.evens.end();
    for (;it!=end; ++it) {
        Cell *cell = *it;
        int energy = (int)cell->getEnergy();
        size_t neighborSize = cell->getNeighbors().size();
        if (neighborSize-energy == energy) {
            cell->toggle();
        }
        parity += cell->getValue();
    }
    currentStepType = (currentStepType == odd) ? even : odd;
}

void Ising::Reservoir::reset() {
    if (constants.getEpsilon()<.7) {
        for (int k = 0; k<clusters; k++) {
            clusterMethod();
        }
    } else {
        for (int k = 0; k<clusters; ++k) {
            metropolisAlgorithm();
        }
    }
}

void Ising::Reservoir::clusterMethod() {
    Randomness::GSLDelegate randomness(RNG);
    ClusterMethodAgent<Randomness::GSLDelegate> cma(randomness,clusterInclusionProbability());
    Cell &currentCell = grid[gsl_rng_uniform_int(RNG, grid.size())];
    cma.performMethodAtCell(currentCell);
}

void Ising::Reservoir::metropolisAlgorithm() {
    Cell &cell = grid[gsl_rng_uniform_int(RNG, grid.size())];
    size_t neighborSize = cell.getNeighbors().size();
    int dE = (int)(neighborSize - cell.getEnergy());
    if ( exp(constants.getBeta() * dE) < gsl_rng_uniform(RNG) ) {
        cell.toggle();
    }
}

void Ising::Reservoir::wheelStep(InteractionResult &result) {
    const char& s1 = interactionCells.first->getValue();
    const char& s2 = interactionCells.second->getValue();
    const char& s3 = parity;
    int inputState = s(s1, s2, s3);
    
    CheckTransitionRuleError(inputState<8 && inputState>=0,
                             InvalidInputStateError);
    
    SystemState *lastState = currentState;
    
    CheckTransitionRuleError(transitions.count(currentState),
                             EmptyTransitionRuleError);
    
    TransitionTable &currentTable = transitions[currentState];
    SystemState *nextState = currentTable[inputState];
    
    CheckTransitionRuleError(currentTable.size() == 8 &&
                                "Transition table size should be 8",
                             InvalidTableSizeError);
    CheckTransitionRuleError(nextState && "nextState must not be null.",
                             TransitionDeadEndError);
    
    const int &oldBit = nextState->bit;
    const int &newBit = currentState->bit;
    
    //The abstraction is leaking, but this saves a very complicated table.
    if (oldBit != newBit) {
        int initialEnergy = (int)(interactionCells.first->getEnergy() \
                                + interactionCells.second->getEnergy() - (s1 ^ s2));
        
        if (newBit>oldBit) {
            interactionCells.first->setValue(0);
            interactionCells.second->setValue(0);
        } else if (newBit < oldBit) {
            interactionCells.first->setValue(0);
            interactionCells.second->setValue(1);
        }
        
        int finalEnergy = (int)(interactionCells.first->getEnergy() \
                                + interactionCells.second->getEnergy() - (s1 ^ s2));
        
        result.work += initialEnergy - finalEnergy;
    }
    currentState = nextState;
}

void Ising::Reservoir::initializeCellsWithRNG(gsl_rng *RNG, int N) {
    reset();
}

Ising::Reservoir::Reservoir(gsl_rng *RNG_, Constants constants, int IS, int cls, TransitionRule rule) :
DemonBase::Reservoir(constants), clusters(cls), grid(IS), RNG(RNG_) {
    parity = 0;
    transitions = rule;
    this->initializeCellsWithRNG(RNG);
    interactionCells.first = &grid[gsl_rng_uniform_int(RNG, grid.size())];
    Cell::Neighbors neighbors = interactionCells.first->getNeighbors();
    interactionCells.second = neighbors[gsl_rng_uniform_int(RNG, neighbors.size())];
}


TransitionRule defaultTransitionRule() {
    //Source: http://imgur.com/FF5TBQh
    
    TransitionRule transitions;
    
    #define TTable(XX) \
        TransitionTable XX;\
        for (char k=0; k<8; k++) \
            XX[k]=&State##XX;
    
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

Ising::Reservoir *Ising::Reservoir::Factory::create(gsl_rng *RNG, Constants constants) {
    return new Ising::Reservoir(RNG,constants,dimension,clusters);
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
        Ising::Reservoir *reservoir = new Ising::Reservoir(RNG,constants,d,clusters);
        reservoir->reset();
        for (int k=0; k<100; k++) {
            printf("%d\n",reservoir->totalEnergy());
            reservoir->reset();
        }
        delete reservoir;
        printf("\n");
    }
}

int Ising::Reservoir::totalEnergy() {
    int energy = 0;
    for (Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        energy+=(int)(*it)->getEnergy();
    }
    // This method overcounts by two, considering both the forward and
    // backword bonds.
    return energy/2;
}
