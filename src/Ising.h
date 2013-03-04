#ifndef ___STOCH___
#define ___STOCH___

#include <utility>
#include <vector>
#include <map>
#include "Reservoir.h"
#include "InstrumentFactories.h"

struct Coordinate {
    int x,y;
    Coordinate(int x_, int y_) : x(x_), y(y_) {}
    Coordinate() : x(0), y(0) {}
};

Coordinate makeCoordinate(int x, int y);

class IsingReservoir : public Reservoir {
public:
    enum stepType {
        odd,
        even
    };
private:
    //Avoid messing with this as much as possible
    char **cells;
    const int isingSide;
    stepType currentStepType;
    typedef std::map<char, SystemState *> TransitionTable;
    std::map<SystemState *,TransitionTable> transitions;
    unsigned char parity;
protected:
    struct Neighbors {
        Coordinate coordinates[4];
    };

    //Methods for accesing stuff:
    Neighbors getNeighbors(Coordinate);
    char &getCell(Coordinate x);
    int countHigh(Neighbors);
    int countHighNeighbors(Coordinate);
    void setupStateTable();
    int getEnergy(Coordinate);

public:
    void isingStep(InteractionResult&);
    void wheelStep(InteractionResult&);
    void clusterMethod();
    int totalEnergy();
    IsingReservoir(gsl_rng *RNG, Constants constants,
                   int IsingSide = 100, int clusters = 20);
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

void isingEnergyDistribution(int d, int clusters);

#endif