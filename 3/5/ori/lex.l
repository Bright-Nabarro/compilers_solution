%{
 LT      
 LE      
 EQ      
 NE      
 GT      
 GE      
 IF      
 THEN    
 ELSE    
 ID      
 NUMBER  
 RELOP   
%}

delim       [ \t\n]
ws          {delim}+
letter_     [A-Za-z_]
digit       [0-9]
id          {letter_}({letter_}|{digit})*
number      ({digit})+(\.{digit}+)?([Ee][+-]?{digit}+)?

%%
{ws}        {}
if          { return IF; }
then        { return THEN; }
else        { return ELSE; }
{id}        { yylval = installID(); return ID; }
{number}    { yylval = installID(); return NUMBER; }
"<"         { yylval = LT; return RELOP; }  
"<="        { yylval = LE; return RELOP; }  
"="         { yylval = EQ; return RELOP; }  
"<>"        { yylval = NE; return RELOP; }  
">"         { yylval = GT; return RELOP; }  
">="        { yylval = GE; return RELOP; }      
%%

int installID()
{
}

int installNum()
{
}
