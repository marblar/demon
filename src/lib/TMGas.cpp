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
#include <boost/iterator/filter_iterator.hpp>
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

void BlockState::update(const TMGas::Block &block) const {
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

void EvolutionRule::operator()(const Block &block) const {
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
    
    using namespace boost;
    typedef std::binder1st< std::not_equal_to<Cell *> > CellFilter;
    CellFilter filter = std::bind1st(std::not_equal_to<Cell *>(),&interactionCell);
    
    BOOST_AUTO(begin, make_filter_iterator(filter, cells.begin()));
    BOOST_AUTO(end, make_filter_iterator(filter, cells.end()));
    
    cellsExcludingInteractionCell.insert(cellsExcludingInteractionCell.begin(), begin, end);
}

void Reservoir::reset() {
    initializeGridWithOccupationProbability(cells, constants.getEpsilon()/2, randomness);
    currentState = DemonBase::randomState();
}

Reservoir::InteractionResult Reservoir::interactWithBit(int bit) {
    const int stepCount = 2;
    InteractionResult result;
    currentState = currentState->stateForBit(bit);
    
    for (int k = 0; k != stepCount; ++k) {
        gridStep();
        wheelStep(result);
    }
    
    result.bit = currentState->bit;
    result.work = currentState->bit - bit;

    return result;
}

void Reservoir::wheelStep(Reservoir::InteractionResult &result) {
    std::pair<bool, bool> bits = hashBits();
    InteractionStateMachine::InputType input(currentState,interactionCell.getValue(),bits.first,bits.second);
    InteractionStateMachine::OutputType output = machine(input);
    
    currentState = output.get<0>();

    interactionCell.setValue(output.get<1>());
}

class EvolutionRuleRef {
    // For some reason, uses for_each directly on the rule causes the member to be optimized out.
    // instead, a new rule is instantiated at every call to grid step
    // --expensive! Adding an extra level of indirection satisfies the compiler's need to delete deallocate something at the end of that function and ensures that only one rule gets created.
    EvolutionRule &rule;
public:
    EvolutionRuleRef(EvolutionRule &_rule) : rule(_rule) {
        
    }
    void operator()(const Block &block) const {
        rule(block);
    }
};


void Reservoir::gridStep() {
    EvolutionRuleRef evolver(rule);
    std::for_each(cells.evenBlocks.begin(), cells.evenBlocks.end(), evolver);
    std::for_each(cells.oddBlocks.begin(), cells.oddBlocks.end(), evolver);
}

std::pair<bool, bool> Reservoir::hashBits() {
    using namespace boost;
    Cell::ValueTransformer transformer;
    BOOST_AUTO(begin, make_transform_iterator(cellsExcludingInteractionCell.begin(), transformer));
    BOOST_AUTO(end, make_transform_iterator(cellsExcludingInteractionCell.end(), transformer));
    std::size_t hash = hash_range(begin, end);
    bool first = hash & 1;
    bool second = (hash >> 1) & 1;
    return std::make_pair(first,second);
}


std::size_t TMGas::hash_value(const TMGas::BlockState& input) {
    boost::hash<TMGas::StateIdentifier> hasher;
    return hasher(input.getStateIdentifier());
}

std::size_t TMGas::hash_value(InteractionStateMachine::InputType const& input)
{
    std::size_t hash = 0;
    boost::hash_combine(hash, input.get<0>());
    boost::hash_combine(hash, input.get<1>());
    boost::hash_combine(hash, input.get<2>());
    boost::hash_combine(hash, input.get<3>());
    return hash;
}

std::size_t TMGas::hash_value(InteractionStateMachine::OutputType const& input)
{
    std::size_t hash = 0;
    boost::hash_combine(hash, input.get<0>());
    boost::hash_combine(hash, input.get<1>());
    return hash;
}

bool TMGas::InteractionStateMachine::InputType::operator==(const TMGas::InteractionStateMachine::InputType &other) const {
    return (get<0>() == other.get<0>()) &&
    (get<1>() == other.get<1>()) &&
    (get<2>() == other.get<2>()) &&
    (get<3>() == other.get<3>());
}

bool TMGas::InteractionStateMachine::OutputType::operator==(const TMGas::InteractionStateMachine::OutputType &other) const {
    return (get<0>() == other.get<0>()) &&
    (get<1>() == other.get<1>());
}

TMGas::InteractionStateMachine::InputType::InputType(DemonBase::SystemState *wheel, bool boltzmann, bool hash1, bool hash2) : boost::tuple<DemonBase::SystemState *, bool, bool, bool>(boost::make_tuple(wheel,boltzmann,hash1,hash2)) {
    
}

TMGas::InteractionStateMachine::OutputType::OutputType(DemonBase::SystemState *wheel, bool boltzmann) : boost::tuple<DemonBase::SystemState *, bool>(boost::make_tuple(wheel,boltzmann)) {
    
}

boost::unordered_set<InteractionStateMachine::InputType> InteractionStateMachine::possibleInputs() const {
    using namespace DemonBase;
    boost::unordered_set<InputType> inputs;
    #define HighAndLowInputs(XX,hash1,hash2) \
        inputs.insert(InputType(&State##XX,hash1,hash2,0));\
        inputs.insert(InputType(&State##XX,hash1,hash2,1));
    
    #define AllStatesFor(XX) \
        HighAndLowInputs(XX,0,0);\
        HighAndLowInputs(XX,0,1);\
        HighAndLowInputs(XX,1,0);\
        HighAndLowInputs(XX,1,1);
    
    AllStatesFor(A1);
    AllStatesFor(B1);
    AllStatesFor(C1);
    AllStatesFor(A0);
    AllStatesFor(B0);
    AllStatesFor(C0);
    
    return inputs;
}

DefaultInteractionMachine::DefaultInteractionMachine() {
    using namespace DemonBase;
    using namespace boost;
    
    #define CommuteRule(XX,a,b,c,YY) \
        table[InputType(&State##XX,a,b,c)]=OutputType(&State##YY,a);\
        table[InputType(&State##YY,a,b,c)]=OutputType(&State##XX,a);

    #define HighAndLowRule(XX,hash1,hash2,YY) \
        CommuteRule(XX,1,hash1,hash2,YY) \
        CommuteRule(XX,0,hash1,hash2,YY)
    
    #define FixedPoints(XX) \
        HighAndLowRule(XX,0,0,XX) \
        HighAndLowRule(XX,0,1,XX) \
        HighAndLowRule(XX,1,0,XX) \
        HighAndLowRule(XX,1,1,XX)
    
    FixedPoints(A1);
    FixedPoints(B1);
    FixedPoints(C1);
    FixedPoints(A0);
    FixedPoints(B0);
    FixedPoints(C0);
    
    // High States:
    //C1 <-> B1
    HighAndLowRule(C1,0, 1,B1);
    
    //B1 <-> A1
    HighAndLowRule(B1, 0, 0, A1);
    
    // A1 -> C0
    table[InputType(&StateA1,0,1,0)] = OutputType(&StateC0,1);
    table[InputType(&StateA1,0,1,1)] = OutputType(&StateC0,1);
    
    // Low states:
    // C0 -> A1
    table[InputType(&StateC0,1,1,1)] = OutputType(&StateA1,0);
    table[InputType(&StateC0,1,1,0)] = OutputType(&StateA1,0);
    
    // A0 <-> B0
    HighAndLowRule(C0, 0, 0, B0);
    
    // B0 <-> C0
    HighAndLowRule(B0, 0, 1, A0);
    
    #undef CommuteRule
    #undef HighAndLowRule
    #undef FixedPoints
}
