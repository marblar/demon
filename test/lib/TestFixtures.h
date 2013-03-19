//
//  MeasurementTest.h
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__MeasurementTest__
#define __jdemon__MeasurementTest__

#define GRID_DIMENSION 6

#include <gsl/gsl_rng.h>
#include <set>
#include <boost/array.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include "Utilities.h"
#include "Ising.h"

struct RandomNumberTestFixture {
    RandomNumberTestFixture() {
        rng = GSLRandomNumberGenerator();
        gsl_rng_set(rng,0);
    }
    ~RandomNumberTestFixture() {
        gsl_rng_free(rng);
    }
    gsl_rng *rng;
};

struct ConstantsTestFixture {
    Constants c;
    ConstantsTestFixture() {
        c.setDelta(.5);
        c.setEpsilon(0);
        c.setTau(1);
        c.setNbits(8);
    }
};

struct EvenIsingGridFixture {
    Ising::Grid grid;
    EvenIsingGridFixture() : grid(GRID_DIMENSION) {}
};

struct ValidStatesTestFixture {
    StateSet validStates;
    ValidStatesTestFixture () {
        validStates = getValidStates();
    }
};

struct DefaultTransitionRuleTestFixture {
    TransitionRule defaultRule;
    DefaultTransitionRuleTestFixture() {
        defaultRule = defaultTransitionRule();
    }
};


// This test fixture is for testing classes that perform operations on Grids. Store the initial values of the cells using resetInitialValues, perform some operation on the grid, and then call changedCells() to get a set of pointers to the cell sites that have changed.
class GridOperationTestFixture : public EvenIsingGridFixture {
    typedef boost::array<unsigned char, GRID_DIMENSION*GRID_DIMENSION> CellValues;
    CellValues initialValues;
    typedef boost::tuple<Ising::Grid::iterator,CellValues::const_iterator> iterator_tuple;
    typedef boost::zip_iterator<iterator_tuple> ZipIterator;
public:
    void resetInitialValues() {
        for (int k = 0; k<initialValues.size(); ++k) {
            initialValues[k]=grid[k]->getValue();
        }
    }
    std::set<Ising::Cell *> changedCells() {
        std::set<Ising::Cell *> changes;
        ZipIterator begin = boost::make_zip_iterator(boost::make_tuple(grid.begin(), initialValues.begin()));
        ZipIterator end = boost::make_zip_iterator(boost::make_tuple(grid.end(),initialValues.end()));
        for (ZipIterator it = begin; it!=end; ++it) {
            Ising::Cell *cell = it->get<0>();
            unsigned char initialValue = it->get<1>();
            if (cell->getValue()!=initialValue) {
                changes.insert(cell);
            }
        }
        return changes;
    }
    GridOperationTestFixture() {
        resetInitialValues();
    }
};


#endif /* defined(__jdemon__MeasurementTest__) */
