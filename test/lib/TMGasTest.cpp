//
//  TMGasTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include <boost/math/tools/fraction.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include "TMGas.h"
#include "TestFixtures.h"
#include "Utilities.h"

// todo: Make this file's organization more clear.

struct TMGasGridFixture : public GridFixture<TMGas::Grid> {
    
};

static inline TMGasGridFixture::CellSet testOverlap(TMGas::Block left, TMGas::Block right) {
    using namespace TMGas;
    TMGasGridFixture::CellSet cellSet;
    BOOST_FOREACH(Cell *leftCell, left) {
        BOOST_FOREACH(Cell *rightCell, right) {
            if (leftCell==rightCell) {
                cellSet.insert(leftCell);
            }
        }
    }
    return cellSet;
}

BOOST_FIXTURE_TEST_SUITE(TMGasGridTests, TMGasGridFixture)

template <bool kind>
class BlockListGetter {
public:
    const TMGas::BlockList &outer;
    const TMGas::BlockList &inner;
    BlockListGetter(const TMGas::Grid &grid) :
        outer(kind ? grid.evenBlocks : grid.oddBlocks),
        inner(kind ? grid.oddBlocks : grid.evenBlocks)
    {}
};

typedef boost::mpl::list<BlockListGetter<true>,BlockListGetter<false> > BlockListGetters;

BOOST_AUTO_TEST_CASE_TEMPLATE(testBlockOverlapSize, BlockListGetter_t, BlockListGetters) {
    // Each even block should be disjoint from each other even block. Same is true for odds.
    typedef TMGas::BlockList::const_iterator iterator;
    typedef std::pair<const TMGas::Block *, CellSet> pair;
    
    BlockListGetter_t block_lists = BlockListGetter_t(grid);
    
    for (iterator outer=block_lists.outer.begin(); outer!=block_lists.outer.end(); ++outer) {
        std::map<const TMGas::Block *,CellSet> overlappingBlock_with_sharedCellSet;
        for (iterator inner=block_lists.inner.begin(); inner!=block_lists.inner.end(); ++inner) {
            CellSet sharedCells = testOverlap(*inner, *outer);
            if (sharedCells.size()>0) {
                overlappingBlock_with_sharedCellSet.insert(std::make_pair(&*inner, sharedCells));
            }
        }
        
        BOOST_CHECK_EQUAL(overlappingBlock_with_sharedCellSet.size(), 4);
        BOOST_FOREACH(pair otherBlock_with_sharedCells, overlappingBlock_with_sharedCellSet) {
            BOOST_CHECK_EQUAL(otherBlock_with_sharedCells.second.size(),1);
        }
    }
}

bool relativePosition(const TMGas::Block &lhs, const TMGas::Block &rhs, bool isAbove, bool isLeft) {
    return  (lhs.isAbove(rhs) == isAbove) && (lhs.isLeft(rhs) == isLeft);
}

class RelativePositionPredicate {
    bool isAbove;
    bool isLeft;
    const TMGas::Block &lhs;
public:
    RelativePositionPredicate(bool _isAbove, bool _isLeft, const TMGas::Block &_lhs) : isAbove(_isAbove), isLeft(_isLeft), lhs(_lhs) {
        
    }
    bool operator()(const TMGas::Block &rhs) const {
        return relativePosition(lhs, rhs,isAbove,isLeft);
    }
};

BOOST_AUTO_TEST_CASE_TEMPLATE(testBlockOverlap, BlockListGetter_t, BlockListGetters) {
    BlockListGetter_t block_lists(grid);
    for (BOOST_AUTO(outer,block_lists.outer.begin()); outer!=block_lists.outer.end(); ++outer) {
        std::map<const TMGas::Block *,CellSet> overlap;
        for (BOOST_AUTO(inner,block_lists.inner.begin()); inner!=block_lists.inner.end(); ++inner) {
            CellSet sharedCells = testOverlap(*inner, *outer);
            if (sharedCells.size()>0) {
                overlap.insert(std::make_pair(&*inner, sharedCells));
            }
        }
        
        BOOST_REQUIRE_EQUAL(overlap.size(),4);
        boost::array<const TMGas::Block *, 4> overlapping_blocks;
        int index = 0;
        BOOST_FOREACH(typeof(*overlap.begin()) leftItem, overlap) {
            overlapping_blocks[index++] = leftItem.first;
        }
        
        const TMGas::Block &middle = *outer;
        typedef boost::array<RelativePositionPredicate, 4> predicateList_t;
        predicateList_t predicates = {{
            RelativePositionPredicate(false, false, middle),
            RelativePositionPredicate(false, true, middle),
            RelativePositionPredicate(true, false, middle),
            RelativePositionPredicate(true, true, middle)
        }};
        
        for (predicateList_t::iterator it = predicates.begin(); it!=predicates.end(); ++it) {
            BOOST_CHECK_EQUAL(1,std::count_if(boost::make_indirect_iterator(overlapping_blocks.begin()), boost::make_indirect_iterator(overlapping_blocks.end()),*it));
        }
    }
}


BOOST_AUTO_TEST_CASE_TEMPLATE( testDisjoint, BlockListGetter_t, BlockListGetters ) {
    BlockListGetter_t block_lists(grid);
    int leftCount = 0;
    int rightCount = 0;
    BOOST_FOREACH(const TMGas::Block &left, block_lists.outer) {
        BOOST_FOREACH(const TMGas::Block &right, block_lists.outer) {
            if (left != right) {
                CellSet shouldBeEmpty = testOverlap(left,right);
                BOOST_CHECK(shouldBeEmpty.empty());
            } else {
                CellSet shouldBeFull = testOverlap(left, right);
                BOOST_CHECK_EQUAL(shouldBeFull.size(), 4);
            }
            ++rightCount;
        }
        ++leftCount;
    }
}

BOOST_AUTO_TEST_CASE( testNotEmpty ) {
    BOOST_CHECK_GT(grid.size(),0);
    BOOST_CHECK_GT(grid.evenBlocks.size(), 0);
    BOOST_CHECK_GT(grid.oddBlocks.size(), 0);
}

BOOST_AUTO_TEST_CASE( testNumberOfBlocks ) {
    BOOST_REQUIRE_EQUAL(grid.evenBlocks.size(),grid.size()/4);
    BOOST_REQUIRE_EQUAL(grid.evenBlocks.size(), grid.size()/4);
}

BOOST_AUTO_TEST_CASE( testBadDimensions ) {
    BOOST_REQUIRE_THROW(TMGas::Grid tooSmall(2), CATools::InvalidGridSize);
    BOOST_REQUIRE_THROW(TMGas::Grid odd(15),CATools::InvalidGridSize);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TMGasGridReservoirTests)

BOOST_FIXTURE_TEST_CASE(testEmptyGridInitialization, RandomGridOperationTestFixture<TMGasGridFixture>) {
    double p = .35;
    int expectedCount = floor(p*grid.size());
    TMGas::initializeGridWithOccupationProbability(grid,p,delegate);
    BOOST_CHECK_EQUAL(expectedCount, changedCells().size());
}

BOOST_FIXTURE_TEST_CASE(testNonEmptyGridInitialization, RandomGridOperationTestFixture<TMGasGridFixture>) {
    double p = .57;
    int expectedCount = floor(p*grid.size());
    grid[5].setValue(true);
    grid[20].setValue(true);
    TMGas::initializeGridWithOccupationProbability(grid, p,delegate);
    BOOST_CHECK_EQUAL(expectedCount, changedCells().size());
}

BOOST_FIXTURE_TEST_CASE(testDifferentGridInitialization, RandomGridOperationTestFixture<TMGasGridFixture>) {
    using namespace TMGas;
    const int iterations = 1000*grid.size();
    
    boost::array<int, 36> array;
    std::fill(array.begin(), array.end(), 0);
    BOOST_REQUIRE_EQUAL(array.size(),grid.size());
    int cellCount = 15;
    
    double p = double(cellCount)/grid.size();
    double expectedRatio = p;
    double variance = expectedRatio*(1-expectedRatio)/iterations;
    double standard_deviation = sqrt(variance);
    double acceptable_error = 3*standard_deviation/expectedRatio;
    BOOST_REQUIRE_LE(acceptable_error,0.05);

    const Cell *base = *grid.begin();
    
    for (int k = 0; k<iterations; ++k) {
        TMGas::initializeGridWithOccupationProbability(grid, 0, delegate);
        resetInitialValues();
        TMGas::initializeGridWithOccupationProbability(grid, p, delegate);
        CellSet changes = changedCells();
        BOOST_REQUIRE_EQUAL(changes.size(), cellCount);
        for (CellSet::iterator it = changes.begin(); it!=changes.end(); ++it) {
            Cell *cell = *it;
            if (cell->getValue()) {
                ++array[cell-base];
            }
        }
    }
    
    for (BOOST_AUTO(it,array.begin()); it!=array.end(); ++it) {
        double actualRatio = (double)(*it)/iterations;
        BOOST_CHECK_CLOSE_FRACTION(expectedRatio, actualRatio, acceptable_error);
    }
}

BOOST_FIXTURE_TEST_CASE(testTooSmallGridInitialization, RandomGridOperationTestFixture<TMGasGridFixture>) {
    double p = 0.001;
    BOOST_REQUIRE_EQUAL((int)(grid.size()*p),0);
    BOOST_REQUIRE_THROW(TMGas::initializeGridWithOccupationProbability(grid, p, delegate), TMGas::InvalidProbabilityError);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TMGasCell)

BOOST_AUTO_TEST_CASE( defaultValue ) {
    TMGas::Cell cell;
    BOOST_REQUIRE(!cell.getValue());
}

BOOST_AUTO_TEST_CASE( toggle ) {
    TMGas::Cell cell;
    bool value = cell.getValue();
    cell.toggle();
    BOOST_CHECK_NE(value, cell.getValue());
}

BOOST_AUTO_TEST_SUITE_END()

class TMGasBlockFixture : public CellTestFixture<TMGas::Cell> {
public:
    TMGas::Block block;
    TMGasBlockFixture() : block(cellReferences) {
        // We want a nice, nontrivial starting state.
        int k = 0;
        for (BOOST_AUTO(it,cellReferences.begin()); it!=cellReferences.end(); ++it) {
            (*it)->setValue(k % 2);
        }
    }
    void setBlockValues_cw(bool tL, bool tR,\
                      bool bL, bool bR) {
        block.topLeft()->setValue(tL);
        block.topRight()->setValue(tR);
        block.bottomRight()->setValue(bR);
        block.bottomLeft()->setValue(bL);
    }
    
    typedef std::pair<TMGas::BlockState,TMGas::BlockState> StatePair;
    template <class RuleClass>
    StatePair applyRule(RuleClass rule) {
        StatePair result;
        result.first=block.currentState();
        rule(block);
        result.second=block.currentState();
        return result;
    };
};


BOOST_FIXTURE_TEST_SUITE(TMGasBlockStates, TMGasBlockFixture)

BOOST_AUTO_TEST_CASE( checkStateCount ) {
    const int unique_states = 1<<block.size();
    std::set<TMGas::StateIdentifier> identifiers;
    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        TMGas::StateIdentifier id = block.currentState().getStateIdentifier();
        identifiers.insert(id);
    }
    
    BOOST_CHECK_EQUAL(unique_states, identifiers.size());
    for (BOOST_AUTO(it,identifiers.begin()); it!=identifiers.end(); ++it) {
        TMGas::BlockState state(*it);
        BOOST_CHECK_EQUAL(state.getStateIdentifier(), *it);
    }

    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        TMGas::BlockState originalState = block.currentState();
        for (BOOST_AUTO(it,cellReferences.begin()); it!=cellReferences.end(); ++it) {
            (*it)->setValue((*it)->getValue());
        }
        originalState.update(block);
        TMGas::BlockState newState = block.currentState();
        BOOST_CHECK_EQUAL_COLLECTIONS(originalState.begin(), originalState.end(), newState.begin(), newState.end());
    }
}

BOOST_AUTO_TEST_CASE( testStateInitialization ) {
    TMGas::BlockState state;
    const TMGas::BlockState nullState(false,false,
                                               false,false);
    for (int k = 0; k != 1<<block.size(); ++k) {
        bool values[4];
        for (int index = 0; index != 4; ++index) {
            values[index] = (k>>index) & 1;
        }
        state.setValuesClockwise(values[0], values[1],
                                 values[2], values[3]);
        
        TMGas::BlockState explicitBlock(values[0],values[1],
                                                 values[2],values[3]);
        BOOST_CHECK(explicitBlock==state);
        
        state.update(block);
        BOOST_CHECK(block.topLeft()->getValue() == values[0]);
        BOOST_CHECK(block.topRight()->getValue() == values[1]);
        BOOST_CHECK(block.bottomLeft()->getValue() == values[2]);
        BOOST_CHECK(block.bottomRight()->getValue() == values[3]);
        nullState.update(block);
    }
}

BOOST_AUTO_TEST_CASE( stateInversion ) {
    TMGas::BlockState blockState;
    TMGas::BlockState invertedState;
    TMGas::BlockState manuallyInverted;
    for (int k = 0; k != 1<<blockState.size(); ++k) {
        for (int index = 0; index<blockState.size(); ++index) {
            bool value = (k>>index) & 1;
            block[index]->setValue(value);
        }
        blockState = block.currentState();
        for (int index = 0; index<blockState.size(); ++index) {
            bool value = !((k>>index) & 1);
            block[index]->setValue(value);
        }
        invertedState = block.currentState();
        BOOST_CHECK((!blockState)==invertedState);
    }
}

BOOST_AUTO_TEST_CASE( stateInvolution ) {
    TMGas::BlockState blockState;
    TMGas::BlockState invertedState;
    TMGas::BlockState manuallyInverted;
    for (int k = 0; k != 1<<blockState.size(); ++k) {
        for (int index = 0; index<blockState.size(); ++index) {
            bool value = (k>>index) & 1;
            block[index]->setValue(value);
        }
        blockState = block.currentState();
        TMGas::BlockState invertedState = !blockState;
        BOOST_CHECK(blockState==!invertedState);
    }
}

BOOST_AUTO_TEST_CASE( isDiagonal ) {
    TMGas::BlockState primaryDiagonal(true,false,
                                               false,true);
    TMGas::BlockState secondaryDiagonal(false,true,
                                                 true,false);
    for (int k = 0; k != 1<<primaryDiagonal.size(); ++k) {
        for (int index = 0; index<primaryDiagonal.size(); ++index) {
            bool value = (k>>index) & 1;
            block[index]->setValue(value);
        }
        if (primaryDiagonal == block.currentState() || secondaryDiagonal == block.currentState()) {
            BOOST_CHECK(block.currentState().isDiagonal());
        } else {
            BOOST_CHECK(!block.currentState().isDiagonal());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

template<class RuleClass>
class TransitionRuleFixture : public TMGasBlockFixture {
public:
    RuleClass rule;
};

// Provided for testing purposes.
class MutableEvolutionRule : public TMGas::EvolutionRule {
public:
    LookupTable &getTable() { return table; }
    MutableEvolutionRule() {
        TMGas::BlockState blockState;
        TMGas::BlockState nullState = TMGas::BlockState(0).getStateIdentifier();
        for (int k = 0; k != 1<<blockState.size(); ++k) {
            for (int index = 0; index<blockState.size(); ++index) {
                blockState[index]=((k>>index) & 1);
            }
            TMGas::StateIdentifier id = blockState.getStateIdentifier();
            table[id] = nullState.getStateIdentifier();
        }
    }
};

BOOST_FIXTURE_TEST_SUITE( TMGasEvolutionRule, TransitionRuleFixture<MutableEvolutionRule> )

BOOST_AUTO_TEST_CASE( testConsistentTransition ) {
    const TMGas::BlockState &newState = rule[block.currentState()];
    rule(block);
    BOOST_CHECK(newState==block.currentState());
}

BOOST_AUTO_TEST_CASE( testExplicitTransition ) {
    TMGas::BlockState nextState(1);
    rule.getTable()[block.currentState().getStateIdentifier()] = nextState.getStateIdentifier();
    rule(block);
    BOOST_CHECK(nextState==block.currentState());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(GenericEvolutionRuleImplementationTests, TMGasBlockFixture)

// KEEP ME UP TO DATE
typedef boost::mpl::list<TMGas::DefaultEvolutionRule> all_rules;

BOOST_AUTO_TEST_CASE_TEMPLATE(oneToOne, Rule, all_rules) {
    Rule rule;
    int unique_states = 1 << block.size();
    std::set<TMGas::StateIdentifier> states;
    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        rule(block);
        BOOST_CHECK_EQUAL(states.count(block.currentState().getStateIdentifier()), 0);
        states.insert(block.currentState().getStateIdentifier());
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(conservationOfEnergy, Rule, all_rules) {
    Rule rule;
    int unique_states = 1 << block.size();
    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        StatePair result = applyRule(rule);
        const TMGas::BlockState &before = result.first;
        const TMGas::BlockState &after = result.second;

        long beforeCount = std::count(before.begin(),before.end(),true);
        long afterCount = std::count(before.begin(),before.end(),true);
        BOOST_CHECK_EQUAL(beforeCount, afterCount);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(notBoring_firstOrder, Rule, all_rules) {
    Rule rule;
    bool notBoring = true;
    int unique_states = 1<<block.size();
    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        StatePair result = applyRule(rule);
        notBoring = notBoring || result.first != result.second;
    }
    BOOST_REQUIRE(notBoring);
}

BOOST_AUTO_TEST_CASE_TEMPLATE( testInvertability, Rule, all_rules) {
    Rule rule;
    for (int k = 0; k != 1<<block.size(); ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        StatePair result = applyRule(rule);
        (!result.first).update(block);
        StatePair invertedResult = applyRule(rule);
        BOOST_REQUIRE(!result.first == invertedResult.first);
        BOOST_CHECK(!result.second == invertedResult.second);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( RotatingEvolutionRuleImplementationTests, TMGasBlockFixture)

typedef boost::mpl::list<TMGas::DefaultEvolutionRule> rotation_rules;

BOOST_AUTO_TEST_CASE_TEMPLATE(testDiagonal_primary, Rule, rotation_rules) {
    Rule rule;
    setBlockValues_cw(true, false,
                      false, true);
    TMGas::BlockState unchanged = block.currentState();
    rule(block);
    BOOST_CHECK(block.currentState()==unchanged);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(testDiagonal_secondary, Rule, rotation_rules) {
    Rule rule;
    setBlockValues_cw(false,true,
                      true,false);
    TMGas::BlockState unchanged = block.currentState();
    rule(block);
    BOOST_CHECK(block.currentState()==unchanged);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(testRotation, Rule, rotation_rules) {
    Rule rule;
    int unique_states = 1<<block.size();
    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        StatePair result = applyRule(rule);
        TMGas::BlockState leftShift;
        TMGas::BlockState rightShift;
        rightShift.setValuesClockwise(block.bottomLeft()->getValue(), block.topLeft()->getValue(),
                                     block.bottomRight()->getValue(), block.topRight()->getValue());
        leftShift.setValuesClockwise(block.topRight()->getValue(), block.bottomRight()->getValue(),
                                     block.topLeft()->getValue(), block.bottomLeft()->getValue());
        BOOST_CHECK(leftShift == result.first || rightShift == result.first || result.first.isDiagonal());
    }
}

BOOST_AUTO_TEST_SUITE_END()

template <int dimension = 10>
class OAReservoirFixture : public ConstantsTestFixture, public RandomNumberTestFixture {
public:
    Randomness::GSLDelegate delegate;
    TMGas::Reservoir reservoir;
    OAReservoirFixture() : delegate(rng), reservoir(c,dimension,delegate) {}
};

BOOST_FIXTURE_TEST_SUITE(TMGasReservoir, OAReservoirFixture<>)

BOOST_AUTO_TEST_CASE(testReset) {
    int iterations = 100;
    int count = 0;
    for (int k = 0; k<iterations; ++k) {
        boost::array<bool,100> before,after;
        BOOST_REQUIRE_EQUAL(before.size(),reservoir.getGrid().size());
        std::transform(reservoir.getGrid().begin(), reservoir.getGrid().end(), before.begin(), TMGas::Cell::ValueTransformer());
        reservoir.reset();
        std::transform(reservoir.getGrid().begin(), reservoir.getGrid().end(), after.begin(), TMGas::Cell::ValueTransformer());
        bool equal = std::equal(before.begin(), before.end(), after.begin());
        count += equal ? 0 : 1;
    }
    // This seems like a good rule of thumb to me.
    BOOST_CHECK_GT(count,iterations*.9);
}

BOOST_AUTO_TEST_CASE(testHashBits) {
    const int iterations = 10000;
    int sum_first = 0;
    int sum_second = 0;
    
    for (int k = 0; k<iterations; ++k) {
        BOOST_AUTO(bits,reservoir.hashBits());
        sum_first += bits.first ? 1 : 0;
        sum_second += bits.second ? 1 : 0;
        reservoir.interactionCell.toggle();
        BOOST_AUTO(shouldBeTheSameBits,reservoir.hashBits());
        BOOST_REQUIRE(bits==shouldBeTheSameBits);
        
        reservoir.reset();
    }
    
    double expectedRatio = .5;
    double variance = expectedRatio*(1-expectedRatio)/iterations;
    double standard_deviation = sqrt(variance);
    
    double acceptable_error = 3*standard_deviation/expectedRatio;
    BOOST_REQUIRE_LE(acceptable_error,0.05);
    
    BOOST_CHECK_CLOSE_FRACTION(static_cast<double>(sum_first)/iterations, expectedRatio, acceptable_error);
    BOOST_CHECK_CLOSE_FRACTION(static_cast<double>(sum_second)/iterations, expectedRatio, acceptable_error);
}

BOOST_AUTO_TEST_CASE( testDoesSomething ) {
    int iterations = 10000;
    int somethingCount = 0;
    for (int k = 0; k<iterations; ++k) {
        int input = 1;//gsl_rng_get(rng) % 2;
        TMGas::Reservoir::InteractionResult result = reservoir.interactWithBit(input);
        somethingCount+=result.bit!=input ? 1 : 0;
        reservoir.reset();
    }
    BOOST_CHECK_GT(somethingCount, iterations*.1);
}


BOOST_AUTO_TEST_CASE( testWheelChangeOnReset ) {
    int iterations = 20000;
    double expectedRatio = 1.0/6.0;
    double variance = expectedRatio*(1-expectedRatio)/iterations;
    double standard_deviation = sqrt(variance);
    double acceptable_error = 3*standard_deviation/expectedRatio;
    BOOST_REQUIRE_LE(acceptable_error,0.05);
    
    std::map<DemonBase::SystemState *, int> counter;
    for (int k = 0; k<iterations; ++k) {
        reservoir.reset();
        ++counter[reservoir.currentState];
    }
    for (BOOST_AUTO(it,counter.begin());it!=counter.end(); ++it) {
        double actualRatio = it->second/static_cast<double>(iterations);
        BOOST_CHECK_CLOSE_FRACTION(actualRatio, expectedRatio, acceptable_error);
    }
}

BOOST_AUTO_TEST_CASE(testWheelChangeOnInteraction) {
    int iterations = 10000;
    int somethingCount = 0;
    for (int k = 0; k<iterations; ++k) {
        int input = 1;//gsl_rng_get(rng) % 2;
        TMGas::Reservoir::InteractionResult result = reservoir.interactWithBit(input);
        somethingCount += result.bit != input ? 1 : 0;
        reservoir.reset();
    }
    // Not really sure what a good number for this one is yet.
    BOOST_CHECK_GT(somethingCount, iterations*.1);
}

BOOST_AUTO_TEST_SUITE_END()

struct TMGasInteractionFixture {
    TMGas::DefaultInteractionMachine rule;
    
    typedef boost::unordered_set<TMGas::DefaultInteractionMachine::InputType> InputSet;
    
    InputSet inputStates;
    
    typedef TMGas::DefaultInteractionMachine::InputType InputType;
    typedef TMGas::DefaultInteractionMachine::OutputType OutputType;
    
    TMGasInteractionFixture() {
        inputStates = rule.possibleInputs();
    }
    
    std::string printInput(const InputType &input) {
        typedef std::string s;
        std::stringstream stream;
        stream << DemonBase::print_state(input.get<0>()) << ","
                << input.get<1>() << ","
                << input.get<2>() << ","
                << input.get<3>();
        return stream.str();
    }
    
    std::string printOutput(const OutputType &output) {
        typedef std::string s;
        std::stringstream stream;
        stream << DemonBase::print_state(output.get<0>()) << ","
                << output.get<1>();
        return stream.str();
    }
};

BOOST_FIXTURE_TEST_SUITE(TMGasInteractionMachine, TMGasInteractionFixture)

BOOST_AUTO_TEST_CASE( testReversibility ) {
    boost::unordered_map<OutputType, InputType> results;
    for (BOOST_AUTO(input, inputStates.begin()); input!=inputStates.end(); ++input) {
        OutputType output(rule(*input));
        
        bool differentHashBits = false;
        std::string failure_string = "";
        if (results.count(output) != 0) {
            InputType input1 = *input;
            InputType input2 = results.at(output);
            failure_string = "Both " + printInput(input1) + " and " + printInput(input2) + " end up at " + printOutput(output);
            differentHashBits = (input1.get<2>() != input2.get<2>()) || (input1.get<3>() != input2.get<3>());
        }
        
        BOOST_CHECK_MESSAGE( results.count(output)==0 || differentHashBits , failure_string );
        results.insert(std::make_pair(output, *input));
    }
}

BOOST_AUTO_TEST_CASE( testProbability ) {
//    BOOST_FAIL("I'm not confident in this test yet.");
    // Test that the transition probabilities are as expected.
    
    boost::unordered_map<std::pair<DemonBase::SystemState *,DemonBase::SystemState *>,double> map;
    
    double epsilon = .1;
    
    for (BOOST_AUTO(input, inputStates.begin()); input!=inputStates.end(); ++input) {
        OutputType output(rule(*input));
        
        double probabilityOfHashBits = .5*.5; // Hash bits are guaranteed to be uniformly distributed by another test.
        double probabilityOfBoltzmannBit = input->get<1>() ? (1 - epsilon)/2 : (1 + epsilon)/2; // Boltzmann bit is guaranteed to be boltzmann distributed by another test.
        
        map[std::make_pair(input->get<0>(), output.get<0>())] += probabilityOfBoltzmannBit*probabilityOfHashBits;
    }
    
    boost::unordered_map<DemonBase::SystemState *, double> outgoingProbablities; // These should all be 1.
    
    double stepDown_probability = 0;
    double stepUp_probability = 0;
    
    for (BOOST_AUTO(it,map.begin()); it!=map.end(); ++it) {
        DemonBase::SystemState *inputState = it->first.first;
        DemonBase::SystemState *outputState = it->first.second;
        double probability = it->second;
        
        outgoingProbablities[inputState]+=probability;
        
        if (inputState == outputState) {
            continue;
        }
        
        // More generally, this should check for detailed balance. Oh well.
        if (inputState->bit == outputState->bit) {
            BOOST_CHECK_CLOSE(probability, .25,.05);
        } else if (inputState->bit < outputState->bit) {
            stepUp_probability += probability;
            
        } else if (inputState->bit > outputState->bit) {
            stepDown_probability += probability;
        }
    }
    
    BOOST_CHECK_CLOSE(stepUp_probability/stepDown_probability,(1-epsilon)/(1+epsilon),.05);
    
    for (BOOST_AUTO(it,outgoingProbablities.begin()); it!=outgoingProbablities.end(); ++it) {
        BOOST_CHECK_CLOSE(it->second,1.0,.05);
    }
}

BOOST_AUTO_TEST_CASE( testConservationOfEnergy ) {
    // The system should only touch the reservoir if it's also writing data to the tape.
    // Here, that means that the boltzmann bit can only change if the tape state changes.
    for (BOOST_AUTO(input, inputStates.begin()); input!=inputStates.end(); ++input) {
        OutputType output(rule(*input));
        
        DemonBase::SystemState *inputState = input->get<0>();
        DemonBase::SystemState *outputState = output.get<0>();
        
        bool inputBoltzmann = input->get<1>();
        bool outputBoltzmann = output.get<1>();
        
        if (inputBoltzmann == outputBoltzmann ) {
            BOOST_CHECK_MESSAGE(inputState->bit==outputState->bit,
                                "Boltzmann: " << inputBoltzmann << "->" << outputBoltzmann <<
                                " with " << DemonBase::print_state(inputState) << "->" << DemonBase::print_state(outputState));
        } else {
            BOOST_CHECK_MESSAGE(inputState->bit!=outputState->bit,
                                "Boltzmann: " << inputBoltzmann << "->" << outputBoltzmann <<
                                " with " << DemonBase::print_state(inputState) << "->" << DemonBase::print_state(outputState));
        }
    }
}

BOOST_AUTO_TEST_CASE( testValidTransitions ) {
    // The transitions must correspond to those permitted by the stochastic reservoir.
    using namespace DemonBase;
    
    for (BOOST_AUTO(input, inputStates.begin()); input!=inputStates.end(); ++input) {
        OutputType output(rule(*input));
        
        DemonBase::SystemState *inputState = input->get<0>();
        DemonBase::SystemState *outputState = output.get<0>();
        BOOST_CHECK_MESSAGE((inputState->nextState1 == outputState || inputState->nextState2 == outputState || inputState == outputState),"Invalid transtion "<<print_state(inputState)<<" -> "<<print_state(outputState));
    }
}

BOOST_AUTO_TEST_CASE( testAllTransitions ) {
    // All transitions permitted by the stochastic reservoir must have a corresponding transition in the TMGas
    using namespace DemonBase;
    
    typedef std::set< std::pair<DemonBase::SystemState *, DemonBase::SystemState *> >  TransitionSet;
    TransitionSet stochTransitions;
    TransitionSet tmTransitions;
    
    SystemState *state;
    
    #define StochTransition(XX) \
        state = &State##XX;\
        if (state!=state->nextState1) \
            stochTransitions.insert(std::make_pair(state,state->nextState1));\
        if (state!=state->nextState2) \
            stochTransitions.insert(std::make_pair(state, state->nextState2));
    
    StochTransition(A1);
    StochTransition(B1);
    StochTransition(C1);
    StochTransition(A0);
    StochTransition(B0);
    StochTransition(C0);
    
    for (BOOST_AUTO(input, inputStates.begin()); input!=inputStates.end(); ++input) {
        OutputType output(rule(*input));
        
        DemonBase::SystemState *inputState = input->get<0>();
        DemonBase::SystemState *outputState = output.get<0>();
        tmTransitions.insert(std::make_pair(inputState, outputState));
    }
    
    for (BOOST_AUTO(it,stochTransitions.begin()); it!=stochTransitions.end(); ++it) {
        BOOST_CHECK_MESSAGE(tmTransitions.count(*it),"No corresponding transition for "<<print_state(it->first)<<"->"<<print_state(it->second));
    }
    #undef StochTransition
}

BOOST_AUTO_TEST_SUITE_END()
