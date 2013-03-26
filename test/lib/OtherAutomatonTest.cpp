//
//  OtherAutomatonTest.cpp
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
#include "OtherAutomaton.h"
#include "TestFixtures.h"

// todo: Make this file's organization more clear.

struct OATestFixture : public GridFixture<OtherAutomaton::Grid> {
    
};

static inline OATestFixture::CellSet testOverlap(OtherAutomaton::Block left, OtherAutomaton::Block right) {
    using namespace OtherAutomaton;
    OATestFixture::CellSet cellSet;
    BOOST_FOREACH(Cell *leftCell, left) {
        BOOST_FOREACH(Cell *rightCell, right) {
            if (leftCell==rightCell) {
                cellSet.insert(leftCell);
            }
        }
    }
    return cellSet;
}

BOOST_FIXTURE_TEST_SUITE(OtherAutomatonGridTests, OATestFixture)

BOOST_AUTO_TEST_CASE( testBlockOverlapSize ) {
    // Each even block should be disjoint from each other even block. Same is true for odds.
    
    typedef OtherAutomaton::BlockList::const_iterator iterator;
    typedef std::pair<const OtherAutomaton::Block *, CellSet> pair;
    
    for (iterator odd=grid.oddBlocks.begin(); odd!=grid.oddBlocks.end(); ++odd) {
        std::map<const OtherAutomaton::Block *,CellSet> overlap;
        for (iterator even=grid.evenBlocks.begin(); even!=grid.evenBlocks.end(); ++even) {
            CellSet sharedCells = testOverlap(*even, *odd);
            if (sharedCells.size()>0) {
                overlap.insert(std::make_pair(&*even, sharedCells));
            }
        }
        BOOST_CHECK_EQUAL(overlap.size(), 2);
        BOOST_FOREACH(pair leftItem, overlap) {
            BOOST_CHECK_EQUAL(leftItem.second.size(),2);
        }
    }
    
    typedef OtherAutomaton::BlockList::const_iterator iterator;
    for (iterator even=grid.evenBlocks.begin(); even!=grid.evenBlocks.end(); ++even) {
        std::map<const OtherAutomaton::Block *,CellSet> overlap;
        for (iterator odd=grid.oddBlocks.begin(); odd!=grid.oddBlocks.end(); ++odd) {
            CellSet sharedCells = testOverlap(*odd, *even);
            if (sharedCells.size()>0) {
                overlap.insert(std::make_pair(&*odd, sharedCells));
            }
        }
        BOOST_CHECK_EQUAL(overlap.size(), 2);
        BOOST_FOREACH(pair leftItem, overlap) {
            BOOST_CHECK_EQUAL(leftItem.second.size(),2);
        }
    }
}

BOOST_AUTO_TEST_CASE( testBlockOverlap ) {
    for (BOOST_AUTO(odd,grid.oddBlocks.begin()); odd!=grid.oddBlocks.end(); ++odd) {
        std::map<const OtherAutomaton::Block *,CellSet> overlap;
        for (BOOST_AUTO(even,grid.evenBlocks.begin()); even!=grid.evenBlocks.end(); ++even) {
            CellSet sharedCells = testOverlap(*even, *odd);
            if (sharedCells.size()>0) {
                overlap.insert(std::make_pair(&*even, sharedCells));
            }
        }
        
        BOOST_REQUIRE_EQUAL(overlap.size(),2);
        boost::array<const OtherAutomaton::Block *, 2> blocks;
        int index = 0;
        BOOST_FOREACH(typeof(*overlap.begin()) leftItem, overlap) {
            blocks[index++] = leftItem.first;
        }
        const OtherAutomaton::Block &middle = *odd;
        
        const int above = 0;
        const int below = 1;
        const int unknown = 2;
        
        boost::array<int, 2> classified;
        for (int k = 0; k!=2; ++k) {
            int c = unknown;
            if (blocks[k]->isAbove(middle)) {
                BOOST_CHECK_EQUAL(c, unknown);
                c = above;
            }
            if (blocks[k]->isBelow(middle)) {
                BOOST_CHECK_EQUAL(c, unknown);
                c = below;
            }
            classified[k]=c;
        }
        size_t aboveCount = std::count(classified.begin(), classified.end(), above);
        size_t belowCount = std::count(classified.begin(), classified.end(), below);
        BOOST_CHECK_EQUAL(aboveCount, 1);
        BOOST_CHECK_EQUAL(belowCount, 1);
    }
}


BOOST_AUTO_TEST_CASE( testDisjoint ) {
    int leftCount = 0;
    int rightCount = 0;
    BOOST_FOREACH(const OtherAutomaton::Block &left, grid.oddBlocks) {
        BOOST_FOREACH(const OtherAutomaton::Block &right, grid.oddBlocks) {
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
    BOOST_REQUIRE_THROW(OtherAutomaton::Grid tooSmall(2), CATools::InvalidGridSize);
    BOOST_REQUIRE_THROW(OtherAutomaton::Grid odd(15),CATools::InvalidGridSize);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OtherAutomatonGridReservoirTests)

BOOST_FIXTURE_TEST_CASE(testEmptyGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    double p = .35;
    int expectedCount = floor(p*grid.size());
    OtherAutomaton::initializeGridWithOccupationProbability(grid,p,delegate);
    BOOST_CHECK_EQUAL(expectedCount, changedCells().size());
}

BOOST_FIXTURE_TEST_CASE(testNonEmptyGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    double p = .57;
    int expectedCount = floor(p*grid.size());
    grid[5].setValue(true);
    grid[20].setValue(true);
    OtherAutomaton::initializeGridWithOccupationProbability(grid, p,delegate);
    BOOST_CHECK_EQUAL(expectedCount, changedCells().size());
}

BOOST_FIXTURE_TEST_CASE(testDifferentGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    using namespace OtherAutomaton;
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
        OtherAutomaton::initializeGridWithOccupationProbability(grid, 0, delegate);
        resetInitialValues();
        OtherAutomaton::initializeGridWithOccupationProbability(grid, p, delegate);
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

BOOST_FIXTURE_TEST_CASE(testTooSmallGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    double p = 0.001;
    BOOST_REQUIRE_EQUAL((int)(grid.size()*p),0);
    BOOST_REQUIRE_THROW(OtherAutomaton::initializeGridWithOccupationProbability(grid, p, delegate), OtherAutomaton::InvalidProbabilityError);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OtherAutomatonCell)

BOOST_AUTO_TEST_CASE( defaultValue ) {
    OtherAutomaton::Cell cell;
    BOOST_REQUIRE(!cell.getValue());
}

BOOST_AUTO_TEST_SUITE_END()

class OtherAutomatonBlockFixture : public CellTestFixture<OtherAutomaton::Cell> {
public:
    OtherAutomaton::Block block;
    OtherAutomatonBlockFixture() : block(cellReferences) {
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
    
    typedef std::pair<OtherAutomaton::BlockState,OtherAutomaton::BlockState> StatePair;
    template <class RuleClass>
    StatePair applyRule(RuleClass rule) {
        StatePair result;
        result.first=block.currentState();
        rule(block);
        result.second=block.currentState();
        return result;
    };
};


BOOST_FIXTURE_TEST_SUITE(OtherAutomatonBlockStates, OtherAutomatonBlockFixture)

BOOST_AUTO_TEST_CASE( checkStateCount ) {
    const int unique_states = 1<<block.size();
    std::set<OtherAutomaton::StateIdentifier> identifiers;
    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        OtherAutomaton::StateIdentifier id = block.currentState().getStateIdentifier();
        identifiers.insert(id);
    }
    
    BOOST_CHECK_EQUAL(unique_states, identifiers.size());
    for (BOOST_AUTO(it,identifiers.begin()); it!=identifiers.end(); ++it) {
        OtherAutomaton::BlockState state(*it);
        BOOST_CHECK_EQUAL(state.getStateIdentifier(), *it);
    }

    for (int k = 0; k < unique_states; ++k) {
        for (int index = 0; index<block.size(); ++index) {
            block[index]->setValue((k>>index) & 1);
        }
        OtherAutomaton::BlockState originalState = block.currentState();
        for (BOOST_AUTO(it,cellReferences.begin()); it!=cellReferences.end(); ++it) {
            (*it)->setValue((*it)->getValue());
        }
        originalState.update(block);
        OtherAutomaton::BlockState newState = block.currentState();
        BOOST_CHECK_EQUAL_COLLECTIONS(originalState.begin(), originalState.end(), newState.begin(), newState.end());
    }
}

BOOST_AUTO_TEST_CASE( testStateInitialization ) {
    OtherAutomaton::BlockState state;
    const OtherAutomaton::BlockState nullState(false,false,
                                               false,false);
    for (int k = 0; k != 1<<block.size(); ++k) {
        bool values[4];
        for (int index = 0; index != 4; ++index) {
            values[index] = (k>>index) & 1;
        }
        state.setValuesClockwise(values[0], values[1],
                                 values[2], values[3]);
        
        OtherAutomaton::BlockState explicitBlock(values[0],values[1],
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
    OtherAutomaton::BlockState blockState;
    OtherAutomaton::BlockState invertedState;
    OtherAutomaton::BlockState manuallyInverted;
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
    OtherAutomaton::BlockState blockState;
    OtherAutomaton::BlockState invertedState;
    OtherAutomaton::BlockState manuallyInverted;
    for (int k = 0; k != 1<<blockState.size(); ++k) {
        for (int index = 0; index<blockState.size(); ++index) {
            bool value = (k>>index) & 1;
            block[index]->setValue(value);
        }
        blockState = block.currentState();
        OtherAutomaton::BlockState invertedState = !blockState;
        BOOST_CHECK(blockState==!invertedState);
    }
}

BOOST_AUTO_TEST_CASE( isDiagonal ) {
    OtherAutomaton::BlockState primaryDiagonal(true,false,
                                               false,true);
    OtherAutomaton::BlockState secondaryDiagonal(false,true,
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
class TransitionRuleFixture : public OtherAutomatonBlockFixture {
public:
    RuleClass rule;
};

// Provided for testing purposes.
class MutableEvolutionRule : public OtherAutomaton::EvolutionRule {
public:
    LookupTable &getTable() { return table; }
    MutableEvolutionRule() {
        OtherAutomaton::BlockState blockState;
        OtherAutomaton::BlockState nullState = OtherAutomaton::BlockState(0).getStateIdentifier();
        for (int k = 0; k != 1<<blockState.size(); ++k) {
            for (int index = 0; index<blockState.size(); ++index) {
                blockState[index]=((k>>index) & 1);
            }
            OtherAutomaton::StateIdentifier id = blockState.getStateIdentifier();
            table[id] = nullState.getStateIdentifier();
        }
    }
};

BOOST_FIXTURE_TEST_SUITE( OtherAutomatonEvolutionRule, TransitionRuleFixture<MutableEvolutionRule> )

BOOST_AUTO_TEST_CASE( testConsistentTransition ) {
    const OtherAutomaton::BlockState &newState = rule[block.currentState()];
    rule(block);
    BOOST_CHECK(newState==block.currentState());
}

BOOST_AUTO_TEST_CASE( testExplicitTransition ) {
    OtherAutomaton::BlockState nextState(1);
    rule.getTable()[block.currentState().getStateIdentifier()] = nextState.getStateIdentifier();
    rule(block);
    BOOST_CHECK(nextState==block.currentState());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(GenericEvolutionRuleImplementationTests, OtherAutomatonBlockFixture)

// KEEP ME UP TO DATE
typedef boost::mpl::list<OtherAutomaton::DefaultEvolutionRule> all_rules;

BOOST_AUTO_TEST_CASE_TEMPLATE(oneToOne, Rule, all_rules) {
    Rule rule;
    int unique_states = 1 << block.size();
    std::set<OtherAutomaton::StateIdentifier> states;
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
        const OtherAutomaton::BlockState &before = result.first;
        const OtherAutomaton::BlockState &after = result.second;

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

BOOST_FIXTURE_TEST_SUITE( RotatingEvolutionRuleImplementationTests, OtherAutomatonBlockFixture)

typedef boost::mpl::list<OtherAutomaton::DefaultEvolutionRule> rotation_rules;

BOOST_AUTO_TEST_CASE_TEMPLATE(testDiagonal_primary, Rule, rotation_rules) {
    Rule rule;
    setBlockValues_cw(true, false,
                      false, true);
    OtherAutomaton::BlockState unchanged = block.currentState();
    rule(block);
    BOOST_CHECK(block.currentState()==unchanged);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(testDiagonal_secondary, Rule, rotation_rules) {
    Rule rule;
    setBlockValues_cw(false,true,
                      true,false);
    OtherAutomaton::BlockState unchanged = block.currentState();
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
        OtherAutomaton::BlockState leftShift;
        OtherAutomaton::BlockState rightShift;
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
    OtherAutomaton::Reservoir reservoir;
    OAReservoirFixture() : reservoir(c,dimension,Randomness::GSLDelegate(rng)) {}
};

BOOST_FIXTURE_TEST_SUITE(OtherAutomatonReservoir, OAReservoirFixture<>)

BOOST_AUTO_TEST_CASE(testReset) {
    int iterations = 100;
    int count = 0;
    for (int k = 0; k<iterations; ++k) {
        boost::array<bool,100> before,after;
        BOOST_REQUIRE_EQUAL(before.size(),reservoir.getGrid().size());
        std::transform(reservoir.getGrid().begin(), reservoir.getGrid().end(), before.begin(), OtherAutomaton::Cell::ValueTransformer());
        reservoir.reset();
        std::transform(reservoir.getGrid().begin(), reservoir.getGrid().end(), after.begin(), OtherAutomaton::Cell::ValueTransformer());
        bool equal = std::equal(before.begin(), before.end(), after.begin());
        count += equal ? 1 : 0;
    }
    // This seems like a good rule of thumb to me.
    BOOST_CHECK_GT(count,iterations*.9);
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(testDoesSomething, 1)
BOOST_AUTO_TEST_CASE( testDoesSomething ) {
    int iterations = 10000;
    int somethingCount = 0;
    for (int k = 0; k<iterations; ++k) {
        int input = gsl_rng_get(rng) % 2;
        OtherAutomaton::Reservoir::InteractionResult result = reservoir.interactWithBit(input);
        somethingCount+=result.bit!=input ? 1 : 0;
    }
    BOOST_CHECK_GT(somethingCount, iterations*.9);
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

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(testDeterministic, 1)
BOOST_AUTO_TEST_CASE( testDeterministic ) {
    BOOST_FAIL("Implement this test before writing the reservoir.");
    int iterations = reservoir.getGrid().size()*reservoir.getGrid().size();
    for (int k = 0; k<iterations; ++k) {
        boost::array<bool,100> before,after;
        BOOST_REQUIRE_EQUAL(before.size(),reservoir.getGrid().size());
        std::transform(reservoir.getGrid().begin(), reservoir.getGrid().end(), before.begin(), OtherAutomaton::Cell::ValueTransformer());
        reservoir.reset();
        std::transform(reservoir.getGrid().begin(), reservoir.getGrid().end(), after.begin(), OtherAutomaton::Cell::ValueTransformer());
        bool equal = std::equal(before.begin(), before.end(), after.begin());
    }
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( testWheelChangeOnInteraction, 1 )
BOOST_AUTO_TEST_CASE(testWheelChangeOnInteraction) {
    int iterations = 10000;
    int somethingCount = 0;
    for (int k = 0; k<iterations; ++k) {
        int input = gsl_rng_get(rng) % 2;
        OtherAutomaton::Reservoir::InteractionResult result = reservoir.interactWithBit(input);
        somethingCount+=result.bit!=input ? 1 : 0;
    }
    // Not really sure what a good number for this one is yet.
    BOOST_CHECK_GT(somethingCount, iterations*.3);
}

BOOST_AUTO_TEST_SUITE_END()
