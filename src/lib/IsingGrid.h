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
#include <boost/iterator/iterator_facade.hpp>

namespace Ising {
    class Cell;
    class Grid;
    class Coordinate;
    class InvalidCellValue;
    class InvalidGridIndex;
    
    class Cell {
        unsigned char value;
    protected:
        friend class Grid;
        boost::array<Cell*, 4> neighbors;
    public:
        Cell() : value(0) { }
        typedef boost::array<Cell*, 4> Neighbors;
        unsigned char &getValue();
        unsigned char setValue(const char &c);
        void toggle() { setValue(value ^ 1); }
        const Neighbors getNeighbors() { return neighbors; }
        const long getEnergy() const;
    };
    
    class InvalidGridIndex : public std::runtime_error {
    public:
        InvalidGridIndex() : std::runtime_error(std::string("Tried to access a cell at an index greater than dimension*dimension.")){};
    };
    
    class Grid {
        int dimension;
        boost::scoped_array<Cell> cells;
        
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
        class iterator : public boost::iterator_facade<iterator,Cell *,boost::forward_traversal_tag> {
        public:
            iterator(bool shouldSkip = false, bool isEnd = false) :
            skips(shouldSkip), end(isEnd) {}
            
            explicit iterator(Cell *n,bool shouldSkip = false, bool isEnd = false) :
            skips(shouldSkip), end(isEnd) {}
        private:
            bool skips;
            bool end;
            Cell *referent;
            friend class boost::iterator_core_access;
            virtual void increment() { skips ? referent+=2 : ++referent; }
            Cell * const & dereference() const { return referent; }
            bool equal(iterator const& other) const {
                return other.end ? referent>=other.referent : referent==other.referent;
            }
        };
        const int &getDimension() const;
        Grid(int dimension);
        
        // Iterators for ising algorithm access.
        iterator allIterator() { return iterator(cells.get()); }
        iterator evenIterator() { return iterator(cells.get(),true); }
        iterator oddIterator() { return iterator(cells.get()+1,true); }
        iterator endIterator() { return iterator(cells.get()+dimension*dimension,false,true); }
        
        size_t size() const { return dimension*dimension; }
        
        Cell * const operator[](size_t gridIndex) {
            if (gridIndex > size()) {
                throw InvalidGridIndex();
            }
            return cells.get()+gridIndex;
        }
    };
    
    class InvalidCellValue : public std::runtime_error {
    public:
        InvalidCellValue() : std::runtime_error(std::string("Tried to set cell to invalid value.")) {}
    };
}



#endif /* defined(__jdemon__IsingGrid__) */
