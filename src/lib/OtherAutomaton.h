//
//  OtherAutomaton.h
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__OtherAutomaton__
#define __jdemon__OtherAutomaton__

#include <vector>
#include <boost/array.hpp>

#include "CATools.h"

namespace OtherAutomaton {
    class Cell : public CATools::Cell<Cell, bool> {
    public:
        inline void _setValue(const bool &val) {
            value = val;
        }
    };

    typedef boost::array<Cell *, 4> Block;
    typedef std::vector<Block> BlockList;
    
    class Grid : public CATools::Grid<Cell> {
        BlockList _evenBlocks;
        BlockList _oddBlocks;
    public:
        Grid(int dimension);
        const BlockList &evenBlocks;
        const BlockList &oddBlocks;
    };
}

#endif /* defined(__jdemon__OtherAutomaton__) */
