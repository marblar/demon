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
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <algorithm>

#include "CATools.h"
#include "RandomnessDelegate.h"
#include "Reservoir.h"
#include "InstrumentFactories.h"

namespace TMGas {
    class Block;
    class Cell;
    class BlockState;
    class Grid;
    class InvalidProbabilityError;
    typedef std::vector<Block> BlockList;
    
    class Cell : public CATools::Cell<Cell, bool> {
    public:
        void toggle() {
            setValue(!getValue());
        }
    };
    
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
        
        void update(const Block &block) const;
        
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
        bool isLeft (const Block &other) const;
        bool isRight(const Block &other) const;
        
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
        typedef boost::unordered_map<const BlockState,BlockState> LookupTable;
    protected:
        LookupTable table;
        EvolutionRule() {}
    public:
        const BlockState operator[](const BlockState &block) const;
        void operator()(const Block &block) const;
    };
    
    class DefaultEvolutionRule : public EvolutionRule {
    public:
        DefaultEvolutionRule();
    };
    
    std::size_t hash_value(const BlockState& input);
    
    class InteractionStateMachine {
    public:
        // This class maps the wheel state, the boltzmann bit, and the two hash bits
        // to a new wheel state and boltzmann bit.
        // See the diagram at https://github.com/marblar/demon/issues/10 for the spec
        
        class InputType : public boost::tuple<DemonBase::SystemState *, // Wheel
        bool,// Boltzmann
        bool,// First hash bit
        bool>// Second hash bit
        {
        public:
            InputType(DemonBase::SystemState *wheel, bool boltzmann, bool hash1, bool hash2);
            bool operator==(const InputType & other) const;
        };
        
        class OutputType : public boost::tuple<DemonBase::SystemState *,// Wheel
        bool>//Boltzmann
        {
        public:
            OutputType(DemonBase::SystemState *wheel, bool boltzmann);
            bool operator==(const OutputType & other) const;
            OutputType() {}
        };
        
        typedef boost::unordered_map<InputType, OutputType> LookupTable;
    protected:
        InteractionStateMachine() {}
        LookupTable table;
    public:
        boost::unordered_set<InputType> possibleInputs() const;
        
        OutputType operator()(const InputType &input) {
            return table.at(input);
        }
    };
    
    class DefaultInteractionMachine : public InteractionStateMachine {
    public:
        DefaultInteractionMachine();
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
        Randomness::GSLDelegate &randomness;
        DefaultInteractionMachine machine;
        DefaultEvolutionRule rule;
        std::vector<Cell *> cellsExcludingInteractionCell;
    public:
        InteractionResult interactWithBit(int bit);
        Cell &interactionCell;
        
        void reset();
        Reservoir(DemonBase::Constants c, int dimension,Randomness::GSLDelegate &delegate);
        std::pair<bool,bool> hashBits();
        const Grid &getGrid() const { return cells; }
        void gridStep();
        void wheelStep(DemonBase::Reservoir::InteractionResult &result);
        
        class Factory : public DemonBase::ReservoirFactory {
            int dimension;
        public:
            Factory(int dim = 16) : dimension(dim) {}
            Reservoir *create(gsl_rng *RNG, DemonBase::Constants constants) {
                return new Reservoir(constants,dimension,*(new Randomness::GSLDelegate(RNG)));
            }
        };
    };

    std::size_t hash_value(InteractionStateMachine::InputType const& input);
    std::size_t hash_value(InteractionStateMachine::OutputType const& input);
    
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
