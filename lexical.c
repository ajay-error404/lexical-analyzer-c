#include "types.h"
#include "lexical.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int paren_count = 0;
int brace_count = 0;

#define MAX_ERRORS 200

typedef struct
{
    int line;
    char message[200];
} ErrorInfo;

ErrorInfo error_list[MAX_ERRORS];
int error_count = 0;

Status check_arguments(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./a.out <filename>\n");
        return e_failure;
    }
    if (strstr(argv[1], ".txt") || strstr(argv[1], ".c") || strstr(argv[1], ".h"))
    {
        printf("Valid input file\n");
        return e_success;
    }

    printf("Invalid file type\n");
    return e_failure;
}


Status do_analyse(const char *filename, Lexer *lexer)
{
    strcpy(lexer->filename, filename);
    lexer->fp = fopen(filename, "r");

    if (lexer->fp == NULL)
    {
        printf("Unable to open file\n");
        return e_failure;
    }

    char line[500];
    int line_num = 1;

    while (fgets(line, sizeof(line), lexer->fp) != NULL)
    {
        process_line(line, line_num);
        line_num++;
    }

    fclose(lexer->fp);
    if (paren_count != 0)
        add_error(-1, "Unclosed parenthesis '('");
    if (brace_count != 0)
        add_error(-1, "Unclosed brace '{'");
    print_all_errors();

    return e_success;
}

void process_line(char *line, int line_number)
{
    int i = 0;
    if(line[i]=='#')
    {
        return;
    }

    while (line[i] != '\0')
    {
        if (line[i] == '(') paren_count++;
        if (line[i] == ')') paren_count--;
        if (line[i] == '{') brace_count++;
        if (line[i] == '}') brace_count--;
        if (line[i] == '\'')
        {
            
            i++;

            if (line[i] == '\0' || line[i+1] != '\'')
            {
                add_error(line_number, "Unclosed or invalid character literal");
                return;
            }

            i += 2;
            continue;
        }

        // Skip whitespace
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')
        {
            i++;
            continue;
        }

        // IDENTIFIER or KEYWORD
        if (isalpha(line[i]) || line[i] == '_')
        {
            char word[100];
            int k = 0;

            while (isalnum(line[i]) || line[i] == '_')
            {
                word[k++] = line[i++];
            }
            word[k] = '\0';

            if (is_keyword(word))
                printf("KEYWORD     : %s\n", word);
            else
                printf("IDENTIFIER  : %s\n", word);

            continue;
        }

        // NUMBER
        if (isdigit(line[i]))
        {
            
            char num[100]; int k = 0;

            while (isalnum(line[i]) || line[i]=='.')
                num[k++] = line[i++];
            num[k]='\0';

            // ERROR: identifier starting with digit
            if (isalpha(num[0]))
            {
                add_error(line_number, "Identifier starting with digit");
                continue;
            }
            int dot_count = 0;
            int len = strlen(num);
            for (int j = 0; j < len; j++)
            {
                if (num[j] == '.')
                    dot_count++;
                if (isalpha(num[j]))
                {
                    if (!((j == len - 1) && (num[j] == 'f' || num[j] == 'F')))
                    {
                        add_error(line_number, "Invalid float (letters inside number)");
                        break;
                    }
                }
            }
            // More than one dot invalid float
            if (dot_count > 1)
            {
                add_error(line_number, "Invalid float (multiple decimal points)");
            }

            if (num[0]=='0' && (num[1]=='x' || num[1]=='X'))
            {
                for (int j=2; num[j]!='\0'; j++)
                {
                    if (!isxdigit(num[j]))
                    {
                        add_error(line_number, "Invalid hexadecimal digit");
                        break;
                    }
                }
            }
            else if (num[0]=='0')
            {
                for (int j=1; num[j]!='\0'; j++)
                {
                    if (num[j]>'7')
                    {
                        add_error(line_number, "Invalid octal digit >= 8");
                        break;
                    }
                }
            }
            if (num[0]=='0' && (num[1]=='b' || num[1]=='B'))
            {
                for (int j=2; num[j]!='\0'; j++)
                {
                    if (num[j] != '0' && num[j] != '1')
                    {
                        add_error(line_number, "Invalid binary digit (>1)");
                        break;
                    }
                }
            }
            printf("NUMBER      : %s\n", num);
            continue;
        }
        // STRING LITERAL
        if (line[i] == '"')
        {
            i++;

            while (line[i] != '"' && line[i] != '\0')
                i++;

            if (line[i] == '\0')
            {
                add_error(line_number, "Unclosed string literal");
                return;
            }

            i++;
            continue;
        }


        // SINGLE-LINE COMMENT //
        if (line[i] == '/' && line[i + 1] == '/')
        {
            
            printf("COMMENT     : %s", &line[i]);
            break;
        }

        // MULTI-CHAR OPERATORS (==, !=, <=, >=, &&, || etc.)
        if (is_operator2(line[i], line[i+1]))
        {
            printf("OPERATOR    : %c%c\n", line[i], line[i+1]);
            i = i + 2;
            continue;
        }

        // SINGLE CHAR OPERATORS
        if (is_operator1(line[i]))
        {
            printf("OPERATOR    : %c\n", line[i]);
            i++;
            continue;
        }

        // PUNCTUATORS
        if (is_punctuator(line[i]))
        {
            printf("PUNCTUATOR  : %c\n", line[i]);
            i++;
            continue;
        }

        i++;
    }
}
int is_keyword(char *word)
{
    const char *keywords[] = 
    {
        "auto","break","case","char","const","continue","default","do",
        "double","else","enum","extern","float","for","goto","if",
        "int","long","register","return","short","signed","sizeof",
        "static","struct","switch","typedef","union","unsigned",
        "void","volatile","while"
    };
    int n = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < n; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

int is_operator1(char ch)
{
    char ops[] = "+-*/%=&|<>!^,.:?[]";
    for (int i = 0; ops[i] != '\0'; i++)
    {
        if (ch == ops[i])
            return 1;
    }
    return 0;
}

int is_operator2(char a, char b)
{
    char *ops[] = {"==", "!=", "<=", ">=", "&&", "||", "++", "--", "+=", "-=", "*=", "/="};
    int n = sizeof(ops) / sizeof(ops[0]);

    char op[3] = {a, b, '\0'};

    for (int i = 0; i < n; i++)
    {
        if (strcmp(op, ops[i]) == 0)
            return 1;
    }
    return 0;
}

int is_punctuator(char ch)
{
    char punc[] = "();{}";
    for (int i = 0; punc[i] != '\0'; i++)
    {
        if (ch == punc[i])
            return 1;
    }
    return 0;
}

void add_error(int line, const char *msg)
{
    if (error_count < MAX_ERRORS)
    {
        error_list[error_count].line = line;
        strcpy(error_list[error_count].message, msg);
        error_count++;
    }
}
void print_all_errors()
{
    if (error_count == 0)
    {
        return;
    }

    printf(RED "\nERRORS : \n" RESET);

    for (int i = 0; i < error_count; i++)
    {
        if (error_list[i].line == -1)
        {
            printf("ERROR: %s\n", error_list[i].message);
            printf("Line %d: %s\n", error_list[i].line, error_list[i].message);
        }
        else
            printf("Line %d: %s\n", error_list[i].line, error_list[i].message);
    }
}





