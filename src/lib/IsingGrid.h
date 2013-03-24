//
//  IsingGrid.h
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__IsingGrid__
#define __jdemon__IsingGrid__

#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/scoped_array.hpp>
#include <boost/array.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include "CATools.h"

namespace Ising {
    class Cell;
    class Grid;
    
    namespace detail {
        // This class is used by the boost::filter_iterator to ensure even
        // contents.
        typedef bool Kind;
        const extern Kind even;
        const extern Kind odd;

        template <class T>
        class CheckerboardPtrOffset {
            T * const base;
            const int dimension;
            const Kind kind;
        public:
            CheckerboardPtrOffset(T * const b, Kind k, int dim) : base(b),dimension(dim), kind(k) {}
            bool operator()(T *n) const;
        };
        typedef CheckerboardPtrOffset<Cell> CheckerboardCellOffset;
    }
    
    class Cell : public CATools::Cell<Cell, unsigned char> {
    public:
        typedef boost::array<Cell*, 4> Neighbors;
    protected:
        friend class Grid;
        Neighbors neighbors;
        
        friend class CATools::Cell<Cell, unsigned char>;
        void _setValue(const char &c);
    public:
        void toggle() { setValue(value ^ 1); }
        Neighbors getNeighbors() { return neighbors; }
        bool isNeighbor(Cell *neighbor){ return std::count(neighbors.begin(),neighbors.end(),neighbor);}
        long getEnergy();
    };
    
    class Grid : public CATools::Grid<Cell> {
    public:
        typedef boost::counting_iterator<Cell *> iterator;
        class subset : public std::vector<Cell *> {
            // This class is a lazy container for iterating over the
            // even or odd cells in the grid. It can be accessed with
            //      Ising:Grid::subset::iterator it = grid.evens.begin();
            detail::Kind kind;
        public:
            detail::Kind getKind() const { return kind; }
            subset(const Grid &grid, detail::Kind k);
        };
        const subset evens;
        const subset odds;
        Grid(int dimension);
    };
    
    class InvalidCellValue : public std::runtime_error {
    public:
        InvalidCellValue() : std::runtime_error(std::string("Tried to set cell to invalid value.")) {}
    };
}

template <class Container>
std::pair<typename Container::iterator, typename Container::iterator> foreach_container(Container &c);



#endif /* defined(__jdemon__IsingGrid__) */
