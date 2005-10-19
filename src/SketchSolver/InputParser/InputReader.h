#ifndef InptReader_h
#define InptReader_h

#include <cstdio>

namespace INp{

extern FILE *yyin;

void Inityylex(void);
void Inityyparse(void);

int yylex(void);

extern int yylineno;
extern char yytext[];
extern int yy_flex_debug;

typedef enum{ INT, LONG, BIT} vartype;

/*
 * This is from y.tab.c
 */
int yyparse(void);

void yyerror(char* );

}

#endif 