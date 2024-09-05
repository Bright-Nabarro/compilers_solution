%{
#include <stdio.h>
%}

delim       [ \t\n]
ws          {delim}+
other       .

%%
{ws}        { printf(" "); }
{other}     { putchar(yytext[0]); }     //lex总是选择最长的前缀，所以其他任意匹配
                                        //需要尽可能短
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
    FILE* file = fopen(argv[1], "r");
    if (!file)
    {
        perror("fopen");
        return 1;
    }

    yyin = file;

    yylex();
}
