//
//  main.cpp
//  FSM
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include <iostream>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf_gamma.h>
#include <assert.h>
#include <time.h>
#include <dispatch/dispatch.h>
#include <map>

#include "States.h"

#define BIT_STREAM_LENGTH 16

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
    setupStates();
    randomShortIntWithBitDistribution(.5,globalRNG);
    Constants constants;
    constants.delta = .5;
    constants.epsilon = 0;
    constants.tau = .1;
    
    System system;
    system.bitPosition = 0;
    system.timeSinceLastBit = 0;
    system.constants = constants;
    
    system.startingBitString = randomShortIntWithBitDistribution(system.constants.delta,globalRNG);
    system.endingBitString = 0;
    system.mass = 0;
    
    time_t start_time = time(NULL);
    
    unsigned int iterations = 1<<27;

    dispatch_semaphore_t histogramSemaphore = dispatch_semaphore_create(1);

    int *histogram = new int[1<<BIT_STREAM_LENGTH];
    int *startingHistogram = new int[1<<BIT_STREAM_LENGTH];
    
    #pragma omp parallel
    {
        System localSystem = system;
        
        gsl_rng *localRNG = GSLRandomNumberGenerator();
        
        int *localHistogram = new int[1<<BIT_STREAM_LENGTH];
        int *localStartingHistogram = new int[1<<BIT_STREAM_LENGTH];
        #pragma omp for
        for (int k=0; k<iterations; ++k) {
            localSystem.bitPosition=0;
            localSystem.timeSinceLastBit=0;
            localSystem.mass=0;
            localSystem.endingBitString=0;
            localSystem.startingBitString = randomShortIntWithBitDistribution(localSystem.constants.delta,localRNG);
            localSystem.currentState = localSystem.startingBitString & 1 ? &StateA1 : &StateA0;
            
            while(localSystem.bitPosition<BIT_STREAM_LENGTH) {
                double rate1 = localSystem.currentState->rate2(localSystem.constants.epsilon);
                
                double rate2 = localSystem.currentState->rate2(localSystem.constants.epsilon);
                
                double fastestTime = gsl_ran_exponential(localRNG, rate1+rate2);
                
                if(fastestTime+localSystem.timeSinceLastBit>constants.tau) {
                    localSystem.timeSinceLastBit = 0;
                    
                    int currentBit = localSystem.currentState->bit;
                    localSystem.endingBitString|=(currentBit<<localSystem.bitPosition);
                    
                    ++localSystem.bitPosition;
                    
                    int newBit = (localSystem.startingBitString >> localSystem.bitPosition) & 1;
                    if(newBit!=localSystem.currentState->bit) {
                        localSystem.currentState = localSystem.currentState->bitFlipState;
                    }
                } else {
                    double probabilityThatState1CameFirst = rate1/(rate1+rate2);
                    
                    SystemState *nextState = probabilityThatState1CameFirst > gsl_rng_uniform(localRNG) ? localSystem.currentState->nextState1 : localSystem.currentState->nextState2;
                    
                    localSystem.mass += localSystem.currentState->bit-nextState->bit;
                    
                    localSystem.currentState = nextState;
                    localSystem.timeSinceLastBit+=fastestTime;
                    localHistogram[localSystem.endingBitString]++;
                    localStartingHistogram[localSystem.startingBitString]++;
                }//End if
            }//End while
            

            
        }//End for
        dispatch_semaphore_wait(histogramSemaphore, DISPATCH_TIME_FOREVER);
        for(int k=0; k<(1<<BIT_STREAM_LENGTH); k++) {
            histogram[localSystem.endingBitString]+=localHistogram[k];
            startingHistogram[localSystem.startingBitString]+=localStartingHistogram[k];
        }
        
        dispatch_semaphore_signal(histogramSemaphore);
    }//End parallel
        
    time_t stop_time = time(NULL);
    
    std::clog<<"Time elapsed: "<<(stop_time-start_time)<<std::endl;
    
    //for(int k=0; k<1<<BIT_STREAM_LENGTH; k++) {
    //    std::cout<<k<<","<<histogram[k]<<std::endl;
    //}
    
}

unsigned int randomShortIntWithBitDistribution(double delta,gsl_rng *randomNumberGenerator) {
    int bits=0;
    for(int k=0; k<BIT_STREAM_LENGTH; k++) {
        int bit = delta > gsl_rng_uniform(randomNumberGenerator);
        bits&=(bit<<k);
    }
    return bits;
}

gsl_rng *GSLRandomNumberGenerator() {
    gsl_rng *gen = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(gen, time(NULL)*getpid());
    return gen;
}
