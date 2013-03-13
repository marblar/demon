//
//  IsingGrid.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "IsingGrid.h"
using namespace Ising;

int Grid::Coordinate::boundsCheck(int x) {
    if (x < 0) {
        return dimension + x;
    }
    if (x >= dimension) {
        return x-dimension;
    }
    return x;
}

Grid::Coordinate::CNeighbors Grid::Coordinate::getNeighbors(){
    Coordinate::CNeighbors neighbors { {
        Coordinate(x,y+1,dimension),
        Coordinate(x,y-1,dimension),
        Coordinate(x+1,y,dimension),
        Coordinate(x-1,y,dimension)
    }};
    return neighbors;
}

Grid::Grid(int dim) : dimension(dim) {
    cells.reset(new Ising::Cell[dimension*dimension]);
    
    // CNeighbors give the neighbors to a coordinate in coordinate space
    // The function getGridIndex translate coordinate space to memory space
    // Cell::Neighbors should give the neighbors to a cell in memory space
    // This procedure boostraps the memory space using coordinate space.
    for (int k = 0; k!=dimension*dimension; ++k) {
        int x = k % dimension;
        int y = k / dimension;
        Coordinate currentCoord(x,y,dimension);
        Cell *cell = cells.get() + currentCoord.getGridIndex();
        
        Coordinate::CNeighbors neighbors = currentCoord.getNeighbors();
        Cell::Neighbors::iterator neighborReference = cell->neighbors.begin();
        for (Coordinate::CNeighbors::iterator it = neighbors.begin();
             it!=neighbors.end();
             ++it) {
            Cell *neighbor = cells.get() + currentCoord.getGridIndex();
            *neighborReference = neighbor;
            ++neighborReference;
        }
    }
}