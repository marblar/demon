#ifndef ___STOCH___
#define ___STOCH___

#include <utility>
#include <vector>
#include <map>
#include "RandomnessDelegate.h"
#include "Reservoir.h"
#include "InstrumentFactories.h"
#include "IsingGrid.h"
#include <stack>

// TODO: Encapsulate me!
typedef std::map<char,SystemState *> TransitionTable;
typedef std::map<SystemState *, TransitionTable> TransitionRule;

TransitionRule defaultTransitionRule();

namespace Ising {
    int s(char x,char y,char z);
}

class IsingReservoir : public Reservoir {
public:
    enum stepType {
        odd,
        even
    };
private:
    Ising::Grid grid;
    stepType currentStepType;
    TransitionRule transitions;
    unsigned char parity;
public:
    void isingStep(InteractionResult&);
    void wheelStep(InteractionResult&);
    void clusterMethod();
    void metropolisAlgorithm();
    int totalEnergy();
    double clusterInclusionProbability() {return 1 - exp(-2*constants.getBeta());}
    IsingReservoir(gsl_rng *RNG, Constants constants,
                   int IsingSide = 20, int clusters = 20,
                   TransitionRule rule = defaultTransitionRule());
    
    //The default is {IsingSide/2,IsingSide/2} and the cell to its right.
    std::pair<Ising::Cell *, Ising::Cell*> interactionCells;
    
    virtual void initializeCellsWithRNG(gsl_rng *RNG, int N = 1<<10);
    virtual InteractionResult interactWithBit(int bit);
    
    class IsingFactory : public ReservoirFactory {
        int dimension, clusters;
    public:
        virtual Reservoir *create(gsl_rng *RNG, Constants constants);
        IsingFactory(int dim, int clst) : dimension(dim), clusters(clst) {}
    };
    int clusters;
    virtual void reset();
    gsl_rng *RNG;
};

class InvalidProbabilityError : public std::runtime_error {
public:
    InvalidProbabilityError() : std::runtime_error("Probability must be 0<p<1") {}
};

template <class RandomnessDelegate>
class ClusterMethodAgent {
    double p;
    RandomnessDelegate &delegate;
public:
    ClusterMethodAgent(RandomnessDelegate &randomDelegate, double inclusionProbability) : p(inclusionProbability), delegate(randomDelegate){
        if (p>1 || p<0) {
            throw InvalidProbabilityError();
        }
    }
    void performMethodAtCell(Ising::Cell *currentCell) {
        //  http://link.springer.com/chapter/10.1007%2F3-540-35273-2_1?LI=true#page-1
        std::stack<Ising::Cell *> workStack;
        unsigned char clusterBit = currentCell->getValue();
        currentCell->toggle();
        workStack.push(currentCell);
        
        //Basic BFT
        while (!workStack.empty()) {
            currentCell = workStack.top();
            workStack.pop();
            Ising::Cell::Neighbors neighbors = currentCell->getNeighbors();
            for (Ising::Cell::Neighbors::iterator it = neighbors.begin(); it!=neighbors.end(); ++it) {
                Ising::Cell *neighborCell = *it;
                if (neighborCell->getValue()==clusterBit && delegate.binaryEventWithProbability(p)) {
                    workStack.push(neighborCell);
                    neighborCell->toggle();
                }
            }
        };
    }
};

#define CheckTransitionRuleError(expr,class) \
    if (!(expr)) { throw class(#expr); }

class InvalidTransitionRuleError : public std::runtime_error {
public:
    InvalidTransitionRuleError(const char *text) : std::runtime_error(text) {}
};

#define InvalidTransitionSubclass(ClassName) \
    class ClassName : public InvalidTransitionRuleError { \
    public: \
        ClassName(const char *text) : InvalidTransitionRuleError(text) {}\
    };

InvalidTransitionSubclass(EmptyTransitionRuleError);
InvalidTransitionSubclass(InvalidInputStateError);
InvalidTransitionSubclass(TransitionDeadEndError);
InvalidTransitionSubclass(InvalidTableSizeError);

#undef InvalidTransitionSubclass

void isingEnergyDistribution(int d, int clusters);

#endif