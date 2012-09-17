//
//  main.cpp
//  thermaleraser
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//  

#include <iostream>
#include <gsl/gsl_randist.h>
#include <time.h>
#include <semaphore.h>

#include "States.h"

struct Constants {
    double delta;
    //The initial ratio of 1's to 0's
    
    double epsilon;
    //The weight of the mass.
    
    double tau;
    //The duration of the interaction with each bit in the stream.
};

struct System {
    int bitPosition;
    double timeSinceLastBit;
    unsigned int startingBitString;
    unsigned int endingBitString;
    int mass;
    SystemState *currentState;
    Constants constants;
};

unsigned int randomShortIntWithBitDistribution(double ratioOfOnesToZeroes, gsl_rng* generator);

gsl_rng *GSLRandomNumberGenerator();

int main(int argc, const char * argv[])
{
    gsl_rng *globalRNG = GSLRandomNumberGenerator();
    
    /*! This call sets up our state machine for the wheel. Each state (i.e. "A0", "C1") is 
     represented by an object with pointers to the next states and the bit-flip states. The
     transition probabilities are computed using ^block objects, so we will need to use a
     compiler with that extension. */
    setupStates();
    
    Constants constants;
    
    /*! The delta used here is NOT, at this point, the delta used in the paper. This is the 
     ratio of ones to zeroes in the bit stream. Probably worth changing the name, but
     calculating this at runtime is just an invitation for bugs. */
    constants.delta = .5;
    constants.epsilon = 0;
    constants.tau = .1;
    
    System system;
    system.bitPosition = 0;
    system.timeSinceLastBit = 0;
    system.constants = constants;
    
    /*! The tape/bitstream is packed into two integers, starting and ending. Bitwise operations
     are used to retrieve starting bits and write the ending bits. This makes our system space 
     the integers from 0 to 2^BIT_STREAM_LENGTH-1, which is handy for storage. */
    
    system.startingBitString = randomShortIntWithBitDistribution(system.constants.delta,globalRNG);
    system.endingBitString = 0;
    system.mass = 0;
    
    time_t start_time = time(NULL);
    
    /*! Use this to change the number of times the tape is simulated */
    unsigned int iterations = 1<<27;
    
    /*! Use this to change the length of the tape. */
    #define BIT_STREAM_LENGTH 16

    /*! The probability of a race condition here is infinitesimal, but so are the performance costs
     of this semaphore. */
    sem_t histogramMutex;
    int ERROR = !sem_init(&histogramMutex, 1, 1);
    if(ERROR) {
        std::cerr<<"Could not create semaphore.\n";
        exit(0);
    }
    
    /*! Putting histograms on the heap protects us from stack overflows when we scale up. */
    int *histogram = new int[1<<BIT_STREAM_LENGTH];
    int *startingHistogram = new int[1<<BIT_STREAM_LENGTH];
    
    #pragma omp parallel
    {
        System localSystem = system;
        
        gsl_rng *localRNG = GSLRandomNumberGenerator();
        
        /* Each thread maintains it's own histogram and reduces them at the end of 
         the simulation. */
        int *localHistogram = new int[1<<BIT_STREAM_LENGTH];
        int *localStartingHistogram = new int[1<<BIT_STREAM_LENGTH];
        
        #pragma omp for
        for (int k=0; k<iterations; ++k) {
            //Reset the system for the start of the new array.
            localSystem.bitPosition=0;
            localSystem.timeSinceLastBit=0;
            localSystem.mass=0;
            localSystem.endingBitString=0;
            localSystem.startingBitString = randomShortIntWithBitDistribution(localSystem.constants.delta,localRNG);
            localSystem.currentState = localSystem.startingBitString & 1 ? &StateA1 : &StateA0;
            
            while(localSystem.bitPosition<BIT_STREAM_LENGTH) {
                /*This line is getting awful ugly. Let me explain: The transition rate is always 1, except
                 when the bit is changing. A transition for which the bit decreases has rate 1-epsilon. A
                 transition where the bit increases is 1 + epsilon. Implementing this with ^blocks was too slow,
                 and subclassing using virtual functions wasn't much better. So I'm doing it inline.*/
                SystemState *currentState = localSystem.currentState;
                double rate1 = 1 + (currentState->nextState1->bit-currentState->bit)*localSystem.constants.epsilon;
                double rate2 = 1 + (currentState->nextState2->bit-currentState->bit)*localSystem.constants.epsilon;
                
                /*! The minimum of two exponentials is an exponential with their combined rates. Since there's a
                 call to log() in this function, we want to use it as few times as possible. So we simply figure out
                 when the next state switch is going ot happen, and if needed we roll again to see
                 which came first. */
                double fastestTime = gsl_ran_exponential(localRNG, rate1+rate2);
                
                if(fastestTime+localSystem.timeSinceLastBit>localSystem.constants.tau) {
                    /*! The tape moved first */
                    
                    localSystem.timeSinceLastBit = 0;
                    
                    int currentBit = localSystem.currentState->bit;
                    localSystem.endingBitString|=(currentBit<<localSystem.bitPosition);
                    
                    ++localSystem.bitPosition;
                    
                    int newBit = (localSystem.startingBitString >> localSystem.bitPosition) & 1;
                    if(newBit!=localSystem.currentState->bit) {
                        localSystem.currentState = localSystem.currentState->bitFlipState;
                    }
                } else {
                    /*! The wheel state changed first.*/
                    double probabilityThatState1CameFirst = rate1/(rate1+rate2);
                    
                    SystemState *nextState = probabilityThatState1CameFirst > gsl_rng_uniform(localRNG) ? localSystem.currentState->nextState1 : localSystem.currentState->nextState2;
                    
                    //Currently the mass is unused...
                    localSystem.mass += localSystem.currentState->bit-nextState->bit;
                    
                    localSystem.currentState = nextState;
                    
                    //It's important to keep this up to date.
                    localSystem.timeSinceLastBit+=fastestTime;
                    
                    localHistogram[localSystem.endingBitString]++;
                    localStartingHistogram[localSystem.startingBitString]++;
                }//End if
            }//End while
        }//End for
        
        /* Here we combine the local histogram into the global histogram. In practice, threads
         almost never reach this phase at the same time. The semaphore protects them from
         potentially colliding and modifying the same bits. */
        sem_wait(&histogramMutex);
        for(int k=0; k<(1<<BIT_STREAM_LENGTH); k++) {
            histogram[localSystem.endingBitString]+=localHistogram[k];
            startingHistogram[localSystem.startingBitString]+=localStartingHistogram[k];
        }
        sem_post(&histogramMutex);
    }//End parallel
    time_t stop_time = time(NULL);
    
    std::clog<<"Time elapsed: "<<(stop_time-start_time)<<std::endl;
    
    //Uncomment this to view the ending histogram.
    //for(int k=0; k<1<<BIT_STREAM_LENGTH; k++) {
    //    std::cout<<k<<","<<histogram[k]<<std::endl;
    //}
    
}

unsigned int randomShortIntWithBitDistribution(double delta,gsl_rng *randomNumberGenerator) {
    int bits=0;
    for(int k=0; k<BIT_STREAM_LENGTH; k++) {
        double randomNumber = gsl_rng_uniform(randomNumberGenerator);
        int bit = delta > randomNumber;
        bits|=(bit<<k);
    }
    return bits;
}

gsl_rng *GSLRandomNumberGenerator() {
    gsl_rng *gen = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(gen, time(NULL)*getpid());
    return gen;
}
