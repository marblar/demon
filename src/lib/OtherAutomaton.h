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

#include "CATools.h"
#include "RandomnessDelegate.h"

namespace OtherAutomaton {
    class Cell : public CATools::Cell<Cell, bool> {
    public:
        inline void _setValue(const bool &val) {
            value = val;
        }
    };

    typedef boost::array<Cell *, 4> Block;
    typedef std::vector<Block> BlockList;
    
    class Grid : public CATools::Grid<Cell> {
        BlockList _evenBlocks;
        BlockList _oddBlocks;
    public:
        Grid(int dimension);
        const BlockList &evenBlocks;
        const BlockList &oddBlocks;
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
            std::clog<<"Warning: No cells will be initialized.";
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
