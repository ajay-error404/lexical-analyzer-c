#ifndef LEXICAL_H
#define LEXICAL_H

#include "types.h"
#include <stdio.h>

typedef struct
{          
    char filename[100]; 
    FILE *fp; 
    int line;             
} Lexer;

Status check_arguments(int argc, char *argv[]);

Status do_analyse(const char *filename, Lexer *lexer);

void process_line(char *line, int line_number);
int is_keyword(char *word);
int is_operator1(char ch);
int is_operator2(char a, char b);
int is_punctuator(char ch);
void add_error(int line, const char *msg);
void print_all_errors();

#endif
