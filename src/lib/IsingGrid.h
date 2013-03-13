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
#include <boost/scoped_array.hpp>
#include <boost/array.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>

namespace Ising {
    class Cell;
    class Grid;
    class Coordinate;
    class InvalidCellValue;
    class InvalidGridIndex;
    
    namespace detail{
        // This class is used by the boost::filter_iterator to ensure even
        // contents.
        template <class T>
        class EvenPtrOffset {
            T * const base;
        public:
            EvenPtrOffset(T const * const b) : base(b) {}
            bool operator()(T *n) const {
                if (n<base) {
                    throw std::runtime_error("EvenPtrOffset only valid for "
                                             "iterators greater than base.");
                }
                return ((n-base)%2)==0;
            }
        };
        typedef EvenPtrOffset<Cell> EvenCellOffset;
    }
    
    class Cell {
        unsigned char value;
    protected:
        friend class Grid;
        boost::array<Cell*, 4> neighbors;
    public:
        Cell() : value(0) { }
        typedef boost::array<Cell*, 4> Neighbors;
        unsigned const char &getValue() { return value; }
        void setValue(const char &c);
        void toggle() { setValue(value ^ 1); }
        Neighbors getNeighbors() { return neighbors; }
        long getEnergy();
    };
    
    class InvalidGridIndex : public std::runtime_error {
    public:
        InvalidGridIndex() : std::runtime_error(std::string("Tried to access a cell at an index greater than dimension*dimension.")){};
    };
    
    class Grid {
        int dimension;
        boost::scoped_array<Cell> cells;
        // ***********************************************
        // ********     Coordinate class       ***********
        // ***********************************************
        class Coordinate {
        protected:
            friend class Grid;
            int boundsCheck(int x);
            size_t getGridIndex() { return dimension * x + y; }
        private:
            typedef boost::array<const Coordinate, 4> CNeighbors;
            int x,y, dimension;
        public:
            CNeighbors getNeighbors();
            Coordinate(int x_, int y_,int dim) :
            x(boundsCheck(x_)), y(boundsCheck(y_)), dimension(dim) {}
            Coordinate(int dim) : x(0), y(0), dimension(dim) {}
            bool operator==(Coordinate &rhs) { return (x==rhs.x) && (y==rhs.y); }
            const int& getX() { return x; }
            const int& getY() { return y; }
            void setX(int x_) { x = boundsCheck(x_); }
            void setY(int y_) { y = boundsCheck(y_); }
        };
        // ************************************************
    public:
        const int &getDimension() const;
        Grid(int dimension);
        size_t size() const { return dimension*dimension; }
        Cell * const operator[](size_t gridIndex) {
            if (gridIndex > size()) {
                throw InvalidGridIndex();
            }
            return cells.get()+gridIndex;
        }
        
        // ******************************
        // ********* Iterators **********
        // ******************************
        typedef boost::counting_iterator<Cell *> iterator;
        class subset {
            // This class is a lazy container for iterating over the
            // even or odd cells in the grid. It can be accessed with
            //      Ising:Grid::subset::iterator it = grid.evens.begin();
            Cell * const base;
            Cell * const last;
        public:
            typedef boost::filter_iterator<detail::EvenCellOffset,Grid::iterator> iterator;
            iterator begin();
            Grid::iterator end();
            size_t size() { throw std::runtime_error("Grid::subset::size() is unimplemented"); }
            subset(Cell *b, Cell *l) : base(b), last(l) {}
        };
        const subset evens;
        const subset odds;
        iterator begin() { return iterator(cells.get()); }
        iterator end() { return iterator(cells.get()+dimension*dimension); }
    };
    
    class InvalidCellValue : public std::runtime_error {
    public:
        InvalidCellValue() : std::runtime_error(std::string("Tried to set cell to invalid value.")) {}
    };
}



#endif /* defined(__jdemon__IsingGrid__) */
