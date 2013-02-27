//
//  Circos.cpp
//  jdemon
//
//  Created by Mark Larus on 2/27/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "Circos.h"
#include "Utilities.h"
#include "System.h"
#include <assert.h>
#include <numeric>
#include <stdlib.h>
#include <sstream>


CircosLogger::CircosLogger(int tapeLength) : dimension(tapeLength+1) {
    links = new unsigned int *[dimension];
    for (int k=0; k<dimension; k++) {
        links[k] = new unsigned int[dimension];
        std::fill(links[k], links[k]+dimension, 0);
    }
};

void CircosLogger::logSystem(System *system) {
    unsigned int start = toIndex(system->startingBitString);
    unsigned int end = toIndex(system->endingBitString);
    links[start][end]++;
}

void CircosLogger::writeLog(std::ostream &output) {
    int linkID = 0;
    int total = 0;
    int *startPos = new int[dimension];
    int *size = new int[dimension];
    int *incomingOffset= new int[dimension];
    int *outGoingOffset = new int[dimension];
    
    std::fill(startPos, startPos+dimension, 0);
    std::fill(size,size+dimension,0);
    std::fill(incomingOffset, incomingOffset+dimension, 0);
    std::fill(outGoingOffset, outGoingOffset+dimension, 0);
    
    for (unsigned int row=0; row<dimension; row++) {
        int start = total;
        startPos[row] = start;
        int _size = std::accumulate(links[row], links[row]+dimension, 0);
        size[row] = _size;
        total += _size + 1;
        output<<chrString(row, start, start+_size);
    }
    
    std::copy(startPos, startPos+dimension, incomingOffset);
    std::copy(startPos, startPos+dimension, outGoingOffset);
    
    for (int row=0; row<dimension; row++) {
        for (int column = 0; column<dimension; column++) {
            output<<linkString(linkID, row, outGoingOffset[row],
                               outGoingOffset[row]+links[row][column]);
            outGoingOffset[row]+=links[row][column];
            output<<linkString(linkID, column, incomingOffset[column],
                               incomingOffset[column]+links[row][column]);
            incomingOffset[column]+=links[row][column];
            linkID++;
        }
    }
    
    delete [] startPos;
    delete [] size;
    delete [] incomingOffset;
    delete [] outGoingOffset;
}

unsigned int CircosLogger::toIndex(unsigned int key) {
    return bitCount(key, dimension-1);
}

std::string CircosLogger::linkString(int linkID, int chrID, int start, int end) {
    std::stringstream s;
    s << "link";
    s << linkID;
    s << " chr";
    s << chrID;
    s << " ";
    s << start;
    s << " ";
    s << end;
    s << "\n";
    return s.str();
}

std::string CircosLogger::chrString(int chrID, int start, int end) {
    std::stringstream output;
    output << "chr";
    output << chrID;
    output << " ";
    output << start;
    output << " ";
    output << end;
    output << "\n";
    return output.str();
}