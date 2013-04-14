//
//  TMGas.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "TMGas.h"
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/functional.hpp>
#include <map>
#include <numeric>
#include <bitset>
#include <functional>
#include "Utilities.h"
using namespace TMGas;

#pragma mark -- Grid Initialization --

Grid::Grid(int dim) : CATools::Grid<Cell>(dim),oddBlocks(_oddBlocks), evenBlocks(_evenBlocks) {
    _oddBlocks.reserve(size());
    _evenBlocks.reserve(size());
    std::insert_iterator<BlockList> oddOutputIterator = std::inserter(_oddBlocks,_oddBlocks.begin());
    std::insert_iterator<BlockList> evenOutputIterator = std::inserter(_evenBlocks,_evenBlocks.begin());
    std::multimap<Cell *, Block *> cellsToBlocks;
    for (int column = 0; column < getDimension(); ++column) {
        for (int row = 0; row < getDimension(); ++row ) {
            if (row % 2 == column % 2) {
                if (row % 2) {
                    const CATools::Coordinate currentCellCoord(column,row,getDimension());
                    *oddOutputIterator = Block(cellsFromCoordinates<CATools::Coordinate::CNeighbors, 4>(currentCellCoord.twoByTwoFromTopLeftCoordinate()));
                } else {
                    CATools::Coordinate currentCoord(column,row,getDimension());
                    *evenOutputIterator = Block(cellsFromCoordinates<CATools::Coordinate::CNeighbors, 4>(currentCoord.twoByTwoFromTopLeftCoordinate()));
                }
            }
        }
    }
}

#pragma mark -- Block Operations --

Block::Block(const boost::array<Cell *, 4>& array) {
    std::copy(array.begin(), array.end(), begin());
}

bool Block::isAbove(const TMGas::Block &above) const {
    return !isBelow(above);
}

bool Block::isBelow(const TMGas::Block &below) const {
    return
        this->topLeft() == below.bottomRight() ||
        this->topLeft() == below.bottomLeft() ||
        this->topRight() == below.bottomLeft() ||
        this->topRight() == below.bottomRight();
}

bool Block::isLeft(const TMGas::Block &other) const {
    // This is a stub so that tests will fail.
    return
        this->bottomRight() == other.topLeft() ||
        this->bottomRight() == other.bottomLeft() ||
        this->topRight() == other.bottomLeft() ||
        this->topRight() == other.bottomRight();
}

bool Block::isRight(const TMGas::Block &other) const {
    return !isLeft(other);
}

const BlockState Block::currentState() const {
    return BlockState(*this);
}

#pragma mark -- BlockState Operations --

BlockState::BlockState(const boost::array<Cell *,4> &cells) {
    std::transform(cells.begin(), cells.end(), begin(), Cell::ValueTransformer());
}

void BlockState::setValuesClockwise(bool topLeft, bool topRight,
                                    bool bottomLeft, bool bottomRight) {
    at(0) = topLeft;
    at(1) = bottomLeft;
    at(2) = topRight;
    at(3) = bottomRight;
}

BlockState::BlockState(const StateIdentifier &state) {
    for (int k = 0; k!=size(); ++k) {
        size_t offset = size() - k - 1;
        at(k) = (state >> offset) & 1;
    }
}

BlockState::BlockState(bool topLeft,bool topRight,bool bottomLeft,bool bottomRight) {
    setValuesClockwise(topLeft, topRight, bottomLeft, bottomRight);
}

void BlockState::update(TMGas::Block &block) const {
    for (int k = 0; k!=size(); ++k) {
        block[k]->setValue(at(k));
    }
}

class AppendBit {
public:
    char operator()(const char &init, const bool& next) const {
        return (init<<1) + (next ? 1 : 0);
    }
};

StateIdentifier BlockState::getStateIdentifier() const {
    StateIdentifier id =  std::accumulate(begin(), end(), 0, AppendBit());
    return id;
}

BlockState BlockState::operator!() const {
    BlockState invertedState;
    std::transform(begin(), end(), invertedState.begin(), std::logical_not<bool>());
    return invertedState;
}

bool BlockState::isDiagonal() const {
    TMGas::BlockState primaryDiagonal(true,false,
                                               false,true);
    TMGas::BlockState secondaryDiagonal(false,true,
                                                 true,false);
    return *this==primaryDiagonal || *this == secondaryDiagonal;
}

#pragma mark -- Evolution Rule --

const BlockState EvolutionRule::operator[](const BlockState &block) const {
    return table.at(block);
}

void EvolutionRule::operator()(Block &block) const {
    BlockState newState = table.at(block.currentState());
    newState.update(block);
}

DefaultEvolutionRule::DefaultEvolutionRule() {
    BlockState start;
    BlockState end;
    
    start.setValuesClockwise(true, false,
                             false, false);
    
    end.setValuesClockwise(false, true,
                             false, false);
    
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
    start.setValuesClockwise(false,true,
                             false,false);
    end.setValuesClockwise(false, false,
                           false, true);
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
    start.setValuesClockwise(false, false,
                             false, true);
    end.setValuesClockwise(false, false,
                             true, false);
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
    start.setValuesClockwise(false,false,
                             true,false);
    end.setValuesClockwise(true, false,
                           false, false);
    
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
    start.setValuesClockwise(false, false,
                             false, false);
    end = start;
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
    start.setValuesClockwise(false, true,
                             true, false);
    end = start;
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
    start.setValuesClockwise(true, false,
                             true, false);
    end.setValuesClockwise(true, true,
                           false, false);
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
    
    start.setValuesClockwise(true,  true,
                             false, false);
    end.setValuesClockwise(false, true,
                           false, true);
    table[start.getStateIdentifier()]=end.getStateIdentifier();
    table[(!start).getStateIdentifier()]=(!end).getStateIdentifier();
    
}

#pragma mark -- Reservoir --

Reservoir::Reservoir(DemonBase::Constants c, int dimension,Randomness::GSLDelegate &delegate) :
DemonBase::Reservoir(c), cells(dimension), randomness(delegate), interactionCell(cells[dimension/2]) {
    initializeGridWithOccupationProbability(cells, c.getEpsilon()/2, delegate);
}

void Reservoir::reset() {
    initializeGridWithOccupationProbability(cells, constants.getEpsilon()/2, randomness);
    currentState = DemonBase::randomState();
}

Reservoir::InteractionResult Reservoir::interactWithBit(int bit) {
    return Reservoir::InteractionResult();
}

std::pair<bool, bool> Reservoir::hashBits() {
    boost::hash<Grid> hasher;
    
    size_t hash = hasher(cells);
    
    bool first = hash & 1;
    bool second = (hash >> 1) & 1;
    return std::make_pair(first,second);
}
