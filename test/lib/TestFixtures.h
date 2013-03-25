//
//  MeasurementTest.h
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__MeasurementTest__
#define __jdemon__MeasurementTest__

#include <gsl/gsl_rng.h>
#include <set>
#include <boost/array.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/typeof/typeof.hpp>
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
    DemonBase::Constants c;
    ConstantsTestFixture() {
        c.setDelta(.5);
        c.setEpsilon(0);
        c.setTau(1);
        c.setNbits(8);
    }
};

struct ValidStatesTestFixture {
    DemonBase::StateSet validStates;
    ValidStatesTestFixture () {
        validStates = DemonBase::getValidStates();
    }
};

struct DefaultTransitionRuleTestFixture {
    TransitionRule defaultRule;
    DefaultTransitionRuleTestFixture() {
        defaultRule = defaultTransitionRule();
    }
};

template <class Grid, int dimension = 6>
struct GridFixture {
    Grid grid;
    GridFixture() : grid(dimension) {}
    typedef Grid gridType;
    typedef std::set<typename Grid::cellType *> CellSet;
};

// This test fixture is for testing classes that perform operations on Grids. Store the initial values of the cells using resetInitialValues, perform some operation on the grid, and then call changedCells() to get a set of pointers to the cell sites that have changed.
template <class GridFixtureBase, int dimension = 6>
class GridOperationTestFixture : public GridFixtureBase {
    typedef boost::array<typename GridFixtureBase::gridType::cellType::valueType, dimension*dimension> CellValues;
    CellValues initialValues;
    typedef boost::tuple<typename GridFixtureBase::gridType::iterator,typename CellValues::const_iterator> iterator_tuple;
public:
    void resetInitialValues() {
        for (int k = 0; k<initialValues.size(); ++k) {
            initialValues[k]=GridFixtureBase::grid[k]->getValue();
        }
    }
    typename GridFixtureBase::CellSet changedCells() {
        typename GridFixtureBase::CellSet changes;
        BOOST_AUTO(begin,boost::make_zip_iterator(boost::make_tuple(GridFixtureBase::grid.begin(), initialValues.begin())));
        BOOST_AUTO(end,boost::make_zip_iterator(boost::make_tuple(GridFixtureBase::grid.end(),initialValues.end())));
        for (BOOST_AUTO(it,begin); it!=end; ++it) {
            typename GridFixtureBase::gridType::cellType *cell = boost::tuples::get<0>(*it);
            unsigned char initialValue = boost::tuples::get<1>(*it);
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

template <class CellType, int N = 4>
class CellTestFixture {
    boost::array<CellType, N> concreteCells;
public:
    boost::array<CellType *, N> cellReferences;
    CellTestFixture() {
        boost::counting_iterator<CellType *> cells(concreteCells.c_array());
        boost::counting_iterator<CellType *> end(concreteCells.c_array()+N);
        std::copy(cells,end,cellReferences.begin());
    }
};

template <class GridFixtureBase>
struct RandomGridOperationTestFixture : public GridOperationTestFixture<GridFixtureBase>, public RandomNumberTestFixture {
    Randomness::GSLDelegate delegate;
    RandomGridOperationTestFixture() : delegate(rng) {}
};

#endif /* defined(__jdemon__MeasurementTest__) */
