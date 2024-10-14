#include "Nfa.h"
#include <stdlib.h>
#include <string.h>


Node* NfaConstruct(Nfa* nfa, Node* root)
{
    switch (root->type)
    {
    case CHAR:
            root->start = nfa->currentState;
            nfa->states[root->start][root->value] = ++nfa->currentState;
            root->end = nfa->currentState++;
            return root;
        break;
    case OP_CONCAT:
        {
            Node* first = NfaConstruct(nfa,root->left);
            Node* second = NfaConstruct(nfa,root->right);
            root->start = nfa->currentState;
            nfa->epsilonTransitions[nfa->currentState][0] = first->start;
            nfa->epsilonTransitions[first->end][0] = second->start;
            nfa->epsilonTransitions[second->end][0] = ++nfa->currentState;
            root->end = nfa->currentState++;
            return root;
        
        break;
        }
    case OP_ALTERNATION:
        {
            Node* first = NfaConstruct(nfa,root->left);
            Node* second = NfaConstruct(nfa,root->right);
            root->start = nfa->currentState;
            nfa->epsilonTransitions[root->start][0] = first->start;
            nfa->epsilonTransitions[root->start][1] = second->start;
            nfa->epsilonTransitions[first->end][0] = ++nfa->currentState;
            nfa->epsilonTransitions[second->end][0] = nfa->currentState;
            root->end = nfa->currentState++;
            return root;
        }
        break;
    case OP_DUPLICATION:
        {
            Node* first = NfaConstruct(nfa, root->left);
            root->start = nfa->currentState;
            nfa->epsilonTransitions[root->start][0] = first->start;
            nfa->epsilonTransitions[first->end][0] = first->start;
            nfa->epsilonTransitions[first->end][1] = ++nfa->currentState;
            root->end = nfa->currentState++;
            nfa->epsilonTransitions[root->start][1] = root->end;
            return root;
        }
        break;
    default:
        err = 1;
        strcpy(error, "ERROR: Invalid node type");
    }

}

void NfaInit(Nfa* nfa, Node* root)
{
    nfa->states = malloc(sizeof(int[256])*256);
    memset(nfa->states, 0, sizeof(int)*256*256);
    nfa->epsilonTransitions = malloc(sizeof(int[2])*256);
    memset(nfa->epsilonTransitions, 0, sizeof(int) * 2 * 256);
    nfa->currentStates = malloc(sizeof(char)*256);
    memset(nfa->currentStates,0,sizeof(char)*256);
    nfa->nextStates = malloc(sizeof(char)*256);
    memset(nfa->nextStates, 0, sizeof(char)*256);
    // 0 is used as error state
    nfa->currentState = 1;
    NfaConstruct(nfa, root);
    nfa->start = root->start;
    nfa->end = root->end;
}


int NfaMatch(Nfa* nfa, const char* string)
{
    memset(nfa->currentStates,0,sizeof(char)*256);
    memset(nfa->nextStates, 0, sizeof(char)*256);
    char stack[256];
    char used[256];
    memset(&used,0,256);
    int top = 0;
    nfa->currentStates[nfa->start] = 1;
    while(*string)
    {
        for(int i = 0; i < 256; i++)
        {
            if(nfa->currentStates[i])
            {
                
                stack[top++] = i;
                while(top != 0)
                {
                    int current = stack[--top];
                    
                    if(nfa->epsilonTransitions[current][0] && !used[nfa->epsilonTransitions[current][0]])
                    {
                        nfa->nextStates[nfa->epsilonTransitions[current][0]] = 1;
                        used[nfa->epsilonTransitions[current][0]] = 1;
                        stack[top++] = nfa->epsilonTransitions[current][0];
                        

                    }
                    if(nfa->epsilonTransitions[current][1] && !used[nfa->epsilonTransitions[current][1]])
                    {
                        nfa->nextStates[nfa->epsilonTransitions[current][1]] = 1;
                        used[nfa->epsilonTransitions[current][1]] = 1;
                        stack[top++] = nfa->epsilonTransitions[current][1];
                        

                    }
                    nfa->nextStates[current] = 0;
                    if(nfa->states[current][*string])
                    {
                       
                        nfa->nextStates[nfa->states[current][*string]] = 1;
                    }
                }
                memset(&used,0,256);
                nfa->currentStates[i] = 0;
            }
        }
        string++;
        char* temp = nfa->currentStates;
        nfa->currentStates = nfa->nextStates;
        nfa->nextStates = temp;
    }

    if(nfa->currentStates[nfa->end])
    {
        return 1;
    }
    for(int i = 0; i < 256; i++)
    {
        if(nfa->currentStates[i])
        {
            
            stack[top++] = i;
            while(top != 0)
            {
                int current = stack[--top];
                
                if(nfa->epsilonTransitions[current][0] && !nfa->nextStates[nfa->epsilonTransitions[current][0]])
                {
                    nfa->nextStates[nfa->epsilonTransitions[current][0]] = 1;
                    stack[top++] = nfa->epsilonTransitions[current][0];
                    

                }
                if(nfa->epsilonTransitions[current][1] && !nfa->nextStates[nfa->epsilonTransitions[current][1]])
                {
                    nfa->nextStates[nfa->epsilonTransitions[current][1]] = 1;
                    stack[top++] = nfa->epsilonTransitions[current][1];
                    

                }
                
            }
        }
    }

    return nfa->nextStates[nfa->end];
}

void NfaPrint(Nfa* nfa)
{
    for(int i = 1; i <= nfa->currentState; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            if(nfa->states[i][j])
            {
                printf("(%d,%d): %d\n", i, j, nfa->states[i][j]);
            }
        }
        if(nfa->epsilonTransitions[i][0])
        {
            printf("%d Epsilion: %d\n", i, nfa->epsilonTransitions[i][0]);
        }
        if(nfa->epsilonTransitions[i][1])
        {
            printf("%d Epsilion: %d\n", i, nfa->epsilonTransitions[i][1]);
        }
    }

    printf("start: %d, end: %d\n", nfa->start, nfa->end);
    printf("current state: %d\n", nfa->currentState);
}

void NfaCleanUp(Nfa* nfa)
{
    free(nfa->states);
    free(nfa->epsilonTransitions);
    free(nfa->currentStates);
    free(nfa->nextStates);
}