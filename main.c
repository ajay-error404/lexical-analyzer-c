/*
-----Description-----
    Name        : Lexical Analyzer Implementation in C
    Date        : 26-12-2025
    AUTHOR      : AJAY KRISHNA M U

    Project Description     :
    Tokens identification such as keywords, identifiers, operators, literals, and symbols.
    Error checking : Unclosed parenthesis , brace , Unclosed or invalid character literal,
                     Identifier starting with digit,Invalid float, Invalid hexadecimal digit,
                     Invalid octal digit, Invalid binary digit , Unclosed string literal,
*/


#include <stdio.h>
#include "types.h"
#include "lexical.h"

int main(int argc, char *argv[])
{
    Lexer lexer;
    if (check_arguments(argc, argv) == e_success)
    {
        if (do_analyse(argv[1], &lexer) == e_success)
        {
            //printf("File opened successfully\n");
        }
        else
        {
            printf("Failed to open file\n");
        }
    }
    else
    {
        printf("Invalid arguments\n");
    }

    return 0;
}
