#ifndef ___STOCH___
#define ___STOCH___

#include <utility>
#include <vector>
#include <map>
#include "Reservoir.h"
#include "ReservoirFactory.h"

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
    int parity;
protected:
    struct Neighbors {
        Coordinate coordinates[4];
    };

    //Methods for accesing stuff:
    Neighbors getNeighbors(Coordinate);
    char &getCell(Coordinate x);
    inline int countHigh(Neighbors);
    inline int countHighNeighbors(Coordinate);
    inline void setupStateTable();
    inline int getEnergy(Coordinate);

    void isingStep(InteractionResult&);
    void wheelStep(InteractionResult&);
public:
    IsingReservoir(gsl_rng *RNG, Constants constants,
                   int IsingSide = 100);
    ~IsingReservoir();
    
    //The default is {IsingSide/2,IsingSide/2} and the cell to its right.
    std::pair<Coordinate, Coordinate> interactionCells;
    
    virtual void initializeCellsWithRNG(gsl_rng *RNG, int N = 1<<10);
    virtual InteractionResult interactWithBit(int bit);
    
    class IsingFactory : public ReservoirFactory {
        int dimension;
    public:
        virtual Reservoir *create(gsl_rng *RNG, Constants constants);
        IsingFactory(int dim) : dimension(dim) {}
    };
};

#endif