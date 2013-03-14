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
    class InvalidCellValue;
    class InvalidGridIndex;
    
    namespace detail{
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
        bool isNeighbor(Cell *neighbor){ return std::count(neighbors.begin(),neighbors.end(),neighbor);}
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
            size_t getGridIndex() const { return dimension * x + y; }
        private:
            typedef boost::array<const Coordinate, 4> CNeighbors;
            int dimension,x,y;
        public:
            CNeighbors getNeighbors();
            Coordinate(int x_, int y_,int dim) : dimension(dim) {
                x = boundsCheck(x_);
                y = boundsCheck(y_);
            }
            Coordinate(int dim) : x(0), y(0), dimension(dim) {}
            bool operator==(Coordinate &rhs) { return (x==rhs.x) && (y==rhs.y); }
            const int& getX() { return x; }
            const int& getY() { return y; }
            void setX(int x_) { x = boundsCheck(x_); }
            void setY(int y_) { y = boundsCheck(y_); }
        };
        // ************************************************
    public:
        const int &getDimension() const { return dimension; }
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
            int dimension;
            detail::Kind kind;
        public:
            typedef boost::filter_iterator<detail::CheckerboardCellOffset,Grid::iterator> iterator;
            iterator begin() const;
            iterator end() const;
            size_t size() { throw std::runtime_error("Grid::subset::size() is unimplemented"); }
            detail::Kind getKind() const { return kind; }
            subset(const Grid &grid, detail::Kind k) : base(*grid.begin()), last(*grid.end()), dimension(grid.getDimension()),kind(k) {}
        };
        const subset evens;
        const subset odds;
        iterator begin() const { return iterator(cells.get()); }
        iterator end() const { return iterator(cells.get()+dimension*dimension); }
    };
    
    class InvalidCellValue : public std::runtime_error {
    public:
        InvalidCellValue() : std::runtime_error(std::string("Tried to set cell to invalid value.")) {}
    };
}



#endif /* defined(__jdemon__IsingGrid__) */
