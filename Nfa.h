#ifndef NFA_H
#define NFA_H

#include "Parser.h"

typedef struct
{
    int (*states)[256];
    // each state has at most two outgoing epsilon transitions
    int (*epsilonTransitions)[2];
    int start;
    int end;
    char* currentStates;
    char* nextStates;
    // used only for construction
    int currentState;
} Nfa;


void NfaInit(Nfa* nfa, Node* root);
int NfaMatch(Nfa* nfa, const char* string);
void NfaPrint(Nfa* nfa);
void NfaCleanUp(Nfa* nfa);

#endif