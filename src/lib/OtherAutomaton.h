//
//  OtherAutomaton.h
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__OtherAutomaton__
#define __jdemon__OtherAutomaton__

#include <vector>
#include <boost/array.hpp>
#include <exception>
#include <string>

#include "CATools.h"
#include "RandomnessDelegate.h"

namespace OtherAutomaton {
    class Cell : public CATools::Cell<Cell, bool> {};

    class Block : public boost::array<Cell *, 4> {
    private:
        friend class Grid;
    public:
        explicit Block(const boost::array<Cell *, 4>& array);
        
        bool isAbove(const Block &other) const;
        bool isBelow(const Block &other) const;
        
        inline Cell *topLeft() const { return at(0); }
        inline Cell *bottomLeft() const { return at(1); }
        inline Cell *bottomRight() const { return at(3); }
        inline Cell *topRight() const { return at(2); }
    };
    
    
    typedef std::vector<Block> BlockList;
    
    class Grid : public CATools::Grid<Cell> {
        BlockList _evenBlocks;
        BlockList _oddBlocks;
    public:
        Grid(int dimension);
        const BlockList &evenBlocks;
        const BlockList &oddBlocks;
    };
    
    class InvalidProbabilityError : public std::runtime_error {
    public:
        InvalidProbabilityError() :
        std::runtime_error(std::string("No cells will be initialized. Use larger p")) {}
    };
    
    template <class RandomnessDelegate>
    void initializeGridWithOccupationProbability(Grid &grid, double probability, RandomnessDelegate &delegate) {
        // Delete all cells.
        for (Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
            Cell *cell = *it;
            cell->setValue(false);
        }
        
        if (!probability) {
            return;
        }
        int occupiedCells = floor(probability*grid.size());
        if(probability && !occupiedCells) {
            throw InvalidProbabilityError();
        }
        
        for (int k = 0; k<occupiedCells; ++k) {
            Cell *cell = grid[delegate];
            while (cell->getValue()) {
                // FIXME: This is inefficient.
                cell = grid[delegate];
            }
            cell->setValue(true);
        }
    }
}

#endif /* defined(__jdemon__OtherAutomaton__) */
