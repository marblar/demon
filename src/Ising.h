#ifndef ___STOCH___
#define ___STOCH___

#include "Reservoir.h"
#include <utility>
#include <vector>
#include <map>

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
    //Convenience types:
    typedef std::vector<Coordinate> CoordinateList;
    typedef CoordinateList::iterator CoordIterator;
    
    //Methods for accesing stuff:
    CoordinateList getNeighbors(Coordinate);
    char &getCell(Coordinate x);
    inline int countHigh(CoordinateList);
    inline int countHighNeighbors(Coordinate);
    inline void setupStateTable();

    int isingStep();
    void wheelStep();
public:
    IsingReservoir(gsl_rng *RNG, Constants constants,
                   int IsingSide = 100);
    ~IsingReservoir();
    
    //The default is {IsingSide/2,IsingSide/2} and the cell to its right.
    std::pair<Coordinate, Coordinate> interactionCells;
    
    virtual void initializeCellsWithRNG(gsl_rng *RNG, int N = 1<<20);
    virtual int interactWithBit(int bit);
};
#endif