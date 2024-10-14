#include <stdlib.h>
#include <string.h>
#include "Parser.h"


int width;
int height;
char* screen;

Node* Parse(Lexer* lexer)
{   
    Node* node = Alternation(lexer);
    if(err)
    {
        NodeCleanUp(node);
        return NULL;
    }
    if (lexer->eos != 1)
    {
        strcpy(error, "ERROR: Couldn't parse entire string\n");
        return NULL;
    }
    return node;

}


Node* Alternation(Lexer* lexer)
{
    Node* node = malloc(sizeof(Node));
    node->left = Concat(lexer);
    if(err)
    {
        return node;
    }
    if(!LexerExpect(lexer, OP_ALTERNATION))
    {
        if(err)
        {
            return node;
        }
        Node* temp = node->left;
        free(node);
        return temp;
    }
    node->right = Concat(lexer);
    if(err)
    {
        return node;
    }

    node->value = '|';
    node->type = OP_ALTERNATION;
    while(LexerExpect(lexer, OP_ALTERNATION))
    {
        if(err)
        {
            return node;
        }
        Node* temp = malloc(sizeof(Node));
        temp->left = node;
        node = temp;
        node->right = Concat(lexer);
        if(err)
        {
            return node;
        }
        node->value = '|';
        node->type = OP_ALTERNATION;
    }
    return node;
    
}

Node* Concat(Lexer* lexer)
{
    Node* node = malloc(sizeof(Node));
    node->left = Duplication(lexer);
    if(err)
    {
        return node;
    }
    if(!LexerExpect(lexer, OP_CONCAT))
    {
        if(err)
        {
            return node;
        }
        Node* temp = node->left;
        free(node);
        return temp;
    }
    node->right = Duplication(lexer);
    if(err)
    {
        return node;
    }
    node->value = '&';
    node->type = OP_CONCAT;
    while(LexerExpect(lexer, OP_CONCAT))
    {
        if(err)
        {
            return node;
        }
        Node* temp = malloc(sizeof(Node));
        temp->left = node;
        node = temp;
        node->right = Duplication(lexer);
        if(err)
        {
            return node;
        }
        node->value = '&';
        node->type = OP_CONCAT;
    }

    return node;
}

Node* Duplication(Lexer* lexer)
{
    Node* node = malloc(sizeof(Node));
    node->left = Paren(lexer);
    if(err)
    {
        return node;
    }
    if(!LexerExpect(lexer, OP_DUPLICATION))
    {
        if(err)
        {
            return node;
        }
        Node* temp = node->left;
        free(node);
        return temp;
    }
    node->value = '*';
    node->type = OP_DUPLICATION;
    while(LexerExpect(lexer, OP_DUPLICATION))
    {
        if(err)
        {
            return node;
        }
        Node* temp = malloc(sizeof(Node));
        temp->left = node;
        node = temp;
        node->value = '*';
        node->type = OP_DUPLICATION;
    }
    return node;
}

Node* Paren(Lexer* lexer)
{
    Node* node;
    if(!LexerExpect(lexer, LPAREN))
    {
        if(err)
        {
            return NULL;
        }
        node = Char(lexer);
        return node;
    }
    node = Alternation(lexer);
    if(err)
    {
        return node;
    }
    if(!LexerExpect(lexer, RPAREN))
    {
        strcpy(error, "ERROR: Expected )\n");
        err = 1;
    }
    return node;
}

Node* Char(Lexer* lexer)
{
    Node* node = malloc(sizeof(Node));
    int charValue;
    if(!(charValue = LexerExpect(lexer, CHAR)))
    {
        err = 1;
        strcpy(error, "ERROR: Expected character\n");
        return node;
    }
    node->type = CHAR;
    node->value = charValue;
    return node;
}


int NodeTreeDepth(Node* root)
{
    if(!root)
    {
        return 0;
    }
    int leftDepth = NodeTreeDepth(root->left);
    int rightDepth = NodeTreeDepth(root->right);
    return leftDepth < rightDepth ? rightDepth + 1 : leftDepth + 1;
}


void NodePrintTreeRecurse(Node* root, int jump, int x, int y)
{
    
    screen[y*width + x] = root->value;
    if(root->left)
    {
        NodePrintTreeRecurse(root->left, jump >> 1, x - jump, y + jump);
        for(int i = 1; i < jump; i++)
        {
            screen[(y+i)*width+(x-i)] = '/';
        }   
    }
    if(root->right)
    {
        NodePrintTreeRecurse(root->right, jump >> 1, x + jump, y + jump);
        for(int i = 1; i < jump; i++)
        {
            screen[(y+i)*width+(x+i)] = '\\';
        }   
    }
}

void NodePrintTree(Node* root)
{
    int depth = NodeTreeDepth(root);
    width = (2 << (depth)) - 1;
    height = (2 << (depth - 1));
    screen = malloc((height*width)*sizeof(char) + 1);
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width - 1; j++)
        {
            screen[i * width + j] = ' ';
        }
        screen[i * width + width - 1] = '\n';
    }

    
    NodePrintTreeRecurse(root, (width >> 2) + 1,width >> 1,0);
    screen[height * width] = '\0';
    printf("%s",screen);
    free(screen);
}



void NodeCleanUp(Node* root)
{
    if(root == NULL)
    {
        return;
    }
    if(root->left != NULL)
    {
        NodeCleanUp(root->left);
        
    }
    if(root->right != NULL)
    {
        NodeCleanUp(root->right);
        
    }
    free(root);
}
