%{
#include <stdio.h>
%}

%%
[0-9]+          { printf("INTEGER: %s\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*  { printf("IDENTIFIER: %s\n", yytext); }
"+"             { printf("PLUS: %s\n", yytext); }
"-"             { printf("MINUS: %s\n", yytext); }
"*"             { printf("MULTIPLY: %s\n", yytext); }
"/"             { printf("DIVIDE: %s\n", yytext); }
[ \t\n]+        { /* 忽略空白符 */ }
.               { printf("UNKNOWN: %s\n", yytext); }
%%

int main() {
    yylex();  // 调用词法分析器
    return 0;
}

int yywrap() {
    return 1;  // 表示输入结束
}
