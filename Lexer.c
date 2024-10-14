#include <ctype.h>
#include <string.h>
#include "Lexer.h"


int err = 0;
char error[128] = {0};
int concat = 0;

int LexerInit(Lexer* lexer, const char* str)
{
    if(!strcmp(str, ""))
    {
        return -1;
    }
    lexer->regex = str;
    lexer->pos = 0;
    lexer->currTok.type = NONE;
    lexer->currTok.c = 0;
    lexer->nextTok.type = NONE;
    lexer->nextTok.c = 0;
    LexerAdvance(lexer);
}

TokenType LexerPeek(Lexer* lexer)
{
    return lexer->currTok.type;
}


int LexerAdvance(Lexer* lexer)
{
    
    
    if(lexer->nextTok.type != NONE)
    {
        lexer->currTok = lexer->nextTok;
        lexer->nextTok.type = NONE;
        return 0;
    }
    if(lexer->regex[lexer->pos] == '\0')
    {
        lexer->eos = 1;
        return 0;
    }
    switch(lexer->regex[lexer->pos])
    {
        case '|':
            lexer->currTok.type = OP_ALTERNATION;
            concat = 0;
            break;
        case '*':
            lexer->currTok.type = OP_DUPLICATION;
            concat = 1;
            break;
        case '(':
                if(concat)
                {
                    lexer->nextTok.type = LPAREN;
                    lexer->currTok.type = OP_CONCAT;
                }
                else
                {
                    lexer->currTok.type = LPAREN;
                }
                concat = 0;
                break;
        case ')':
            lexer->currTok.type = RPAREN;
            concat = 1;
            break;
        default:
            if(isalnum(lexer->regex[lexer->pos]))
            {
                if(concat)
                {
                    lexer->currTok.type = OP_CONCAT;
                    lexer->nextTok.type = CHAR;
                    lexer->nextTok.c = lexer->regex[lexer->pos];
                }
                else
                {
                    lexer->currTok.type = CHAR;
                    lexer->currTok.c = lexer->regex[lexer->pos];
                }
                concat = 1;
                break;
            }
            err = 1;
            sprintf(error, "ERROR: Invalid character: %c", lexer->regex[lexer->pos]);
            return 0;
    }
    lexer->pos++;
    return 0;
}


char LexerExpect(Lexer* lexer, TokenType type)
{
    if(lexer->eos)
    {
        return 0;
    }
    TokenType currType = LexerPeek(lexer);
    char c = lexer->currTok.c;
    if(currType == type)
    {
        
        LexerAdvance(lexer);
        if(err)
        {
            return 0;
        }
        return c == 0 ? 1 : c;
    }
    return 0;

}


