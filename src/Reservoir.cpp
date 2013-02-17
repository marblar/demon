void setupStates() {
    static bool alreadyInitialized = false;
    
    if(!alreadyInitialized) {
        
        StateA1.nextState1 = &StateB1;
        StateA1.nextState2 = &StateC0;
        StateA1.bitFlipState = &StateA0;
        StateA1.bit = 1;
        StateA1.letter = 'A';
        
        StateB1.nextState1 = &StateA1;
        StateB1.nextState2 = &StateC1;
        StateB1.bitFlipState = &StateB0;
        StateB1.bit = 1;
        StateB1.letter = 'B';
        
        StateC1.nextState1 = &StateB1;
        StateC1.nextState2 = &StateC1;
        StateC1.bitFlipState = &StateC0;
        StateC1.bit = 1;
        StateC1.letter = 'C';
        
        StateA0.nextState1 = &StateB0;
        StateA0.nextState2 = &StateA0;
        StateA0.bitFlipState = &StateA1;
        StateA0.bit = 0;
        StateA0.letter = 'A';
        
        StateB0.nextState1 = &StateA0;
        StateB0.nextState2 = &StateC0;
        StateB0.bitFlipState = &StateB1;
        StateB0.bit = 0;
        StateB0.letter = 'B';
        
        StateC0.nextState1 = &StateB0;
        StateC0.nextState2 = &StateA1;
        StateC0.bitFlipState = &StateC1;
        StateC0.bit = 0;
        StateC0.letter = 'C';
    }
}

SystemState *randomState() {
    int index = rand()%6;
    SystemState *randomState[] = {&StateA0,&StateA1,&StateB0,&StateB1,&StateB0,&StateC0,&StateC1};
    return randomState[index];
}