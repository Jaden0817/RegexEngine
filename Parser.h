#ifndef PARSER_H
#define PARSER_H
#include "Lexer.h"


typedef struct Node Node;

struct Node
{
    char value;
    TokenType type;
    Node* left;
    Node* right;

    // used for construction
    int start;
    int end;
};

Node* Parse(Lexer* lexer);
Node* Alternation(Lexer* lexer);
Node* Concat(Lexer* lexer);
Node* Duplication(Lexer* lexer);
Node* Paren(Lexer* lexer);
Node* Char(Lexer* lexer);

int NodeTreeDepth(Node* root);
void NodePrintTree(Node* root);
void NodeCleanUp(Node* root);

#endif