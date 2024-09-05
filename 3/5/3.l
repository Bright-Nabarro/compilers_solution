%{
#include <stdio.h>
FILE* outfile = NULL;
%}

delim       [ \t\n]
ws          {delim}+
letter_     [A-Za-z_]
digit       [0-9]
id          {letter_}({letter_}|{digit})*
float       (float)
other       .

%%
{ws}        { fprintf(outfile, "%*s", yyleng, &yytext[0]); }
{float}     { fprintf(outfile, "%s", "double"); }
{id}        { fprintf(outfile, "%*s", yyleng, &yytext[0]); }
{other}     { fprintf(outfile, "%c", yytext[0]); }
%%

int yywrap()
{
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "need a C file\n");
        return 1;
    }

    FILE* infile = fopen(argv[1], "r");
    if (!infile)
    {
        perror("infile fopen failed");
        return 1;
    }
    
    outfile = fopen("output.c", "w");
    if (!outfile)
    {
        perror("output fopen filed");
        return 1;
    }

    yyin = infile;

    yylex();
}
