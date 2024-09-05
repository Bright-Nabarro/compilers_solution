%{
#include <stdio.h>
FILE* outfile = NULL;
%}

delim       [ \t\n]
ws          {delim}+
vowel       [aeiouAEIOU]
constant    [bcdfghjklmnpqrstvwxyzBCDFGHIJKLMNPQRSTVWXYZ]
vowel_word  {vowel}[a-zA-Z]*
const_word  {constant}[a-zA-Z]*
other       .

%%
{ws}            { fprintf(outfile, "%*s", yyleng, &yytext[0]); }
{vowel_word}    { fprintf(outfile, "%*s%c%s", yyleng-1, &yytext[1], yytext[0], "ay"); }
{const_word}    { fprintf(outfile, "%*s%s", yyleng, &yytext[0], "ay"); }
{other}         { fprintf(outfile, "%c", yytext[0]); }
%%

int yywrap()
{
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "need a file\n");
        return 1;
    }
    FILE* infile = fopen(argv[1], "r");
    if (!infile)
    {
        perror("infile fopen");
        return 1;
    }

    outfile = fopen("4.l.out.txt", "w");
    if (!outfile)
    {
        perror("outfile fopen");
        return 1;
    }

    yyin = infile;
    yylex();
}



