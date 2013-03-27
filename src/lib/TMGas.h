//
//  TMGas.h
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__TMGas__
#define __jdemon__TMGas__

#include <vector>
#include <boost/array.hpp>
#include <exception>
#include <string>
#include <map>
#include <set>
#include <boost/multi_array.hpp>
#include <algorithm>

#include "CATools.h"
#include "RandomnessDelegate.h"
#include "Reservoir.h"

namespace TMGas {
    class Block;
    class Cell;
    class BlockState;
    class Grid;
    class InvalidProbabilityError;
    typedef std::vector<Block> BlockList;
    
    class Cell : public CATools::Cell<Cell, bool> {};
    
    typedef unsigned short int StateIdentifier;
    
    class BlockState : public boost::array<bool,4> {
    public:
        BlockState(const boost::array<Cell *,4> &);
        BlockState(const StateIdentifier &state);
        BlockState(bool,bool,bool,bool);
        
        bool operator ==(const BlockState &other) const {
            return std::equal(begin(), end(), other.begin(), std::equal_to<bool>());
        }
        
        BlockState() {}
        StateIdentifier getStateIdentifier() const;
        
        
        void setValuesClockwise(bool topLeft,    bool topRight,
                                bool bottomLeft, bool bottomRight);
        
        void update(Block &block) const;
        
        BlockState operator!() const;
        bool isDiagonal() const;
    };

    class Block : public boost::array<Cell *, 4> {
    private:
        friend class Grid;
    public:
        explicit Block(const boost::array<Cell *, 4>& array);
        
        bool isAbove(const Block &other) const;
        bool isBelow(const Block &other) const;
        
        inline const Cell *topLeft() const { return at(0); }
        inline const Cell *bottomLeft() const { return at(1); }
        inline const Cell *bottomRight() const { return at(3); }
        inline const Cell *topRight() const { return at(2); }
        
        inline Cell *topLeft() { return at(0); }
        inline Cell *bottomLeft() { return at(1); }
        inline Cell *bottomRight() { return at(3); }
        inline Cell *topRight() { return at(2); }
        
        const BlockState currentState() const;
    };
    
    class EvolutionRule {
    public:
        typedef std::map<StateIdentifier,StateIdentifier> LookupTable;
    protected:
        LookupTable table;
        EvolutionRule() {}
    public:
        const BlockState operator[](const BlockState &block) const;
        void operator()(Block &block) const;
    };
    
    class DefaultEvolutionRule : public EvolutionRule {
    public:
        DefaultEvolutionRule();
    };
    
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
    
    class Reservoir : public DemonBase::Reservoir {
        Grid cells;
    public:
        InteractionResult interactWithBit(int bit) { return InteractionResult(); }
        void reset() {}
        Reservoir(DemonBase::Constants c, int dimension,const Randomness::GSLDelegate &delegate) : DemonBase::Reservoir(c),cells(dimension) {}
        const Grid &getGrid() const { return cells; }
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
        
        
        Cell *cell = &grid[delegate];
        for (int k = 0; k<occupiedCells; ++k) {
            while (cell->getValue()) {
                // FIXME: This is inefficient.
                cell = &grid[delegate];
            }
            cell->setValue(true);
        }
    }
}

#endif /* defined(__jdemon__TMGas__) */
