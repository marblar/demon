#ifndef ___STOCH___
#define ___STOCH___

#include <utility>
#include <vector>
#include <map>
#include "Reservoir.h"
#include "InstrumentFactories.h"
#include "IsingGrid.h"

struct Coordinate;
// TODO: Encapsulate me!
typedef std::map<char,SystemState *> TransitionTable;
typedef std::map<SystemState *, TransitionTable> TransitionRule;

TransitionRule defaultTransitionRule();

namespace ising {
    int s(char x,char y,char z);
    int boundsCheck(int x, const int max);
}

class IsingReservoir : public Reservoir {
public:
    enum stepType {
        odd,
        even
    };
    struct Neighbors {
        Coordinate coordinates[4];
    };
private:
    //Avoid messing with this as much as possible
    char **cells;
    const int isingSide;
    stepType currentStepType;
    TransitionRule transitions;
    unsigned char parity;
protected:
    //Methods for accesing stuff:
    char &getCell(Coordinate x);
    int countHigh(Neighbors);
    int countHighNeighbors(Coordinate);
    int getEnergy(Coordinate);

public:
    void isingStep(InteractionResult&);
    void wheelStep(InteractionResult&);
    void clusterMethod();
    void metropolisAlgorithm();
    int totalEnergy();
    IsingReservoir(gsl_rng *RNG, Constants constants,
                   int IsingSide = 20, int clusters = 20,
                   TransitionRule rule = defaultTransitionRule());
    ~IsingReservoir();
    
    //The default is {IsingSide/2,IsingSide/2} and the cell to its right.
    std::pair<Coordinate, Coordinate> interactionCells;
    
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

IsingReservoir::Neighbors getNeighbors(Coordinate,int);

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