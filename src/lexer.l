%{
/************************************************************************
 * File:	lexer.l							*
 ************************************************************************/

# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>

# include "expressions.h"
# include "parser.h"

// # undef atof
// extern double atof ( );

char *lexinput;
// extern Code expression;
// extern void yyparse(void);

# undef  YY_INPUT
# define YY_INPUT(buf,result,max_size) \
	    result = (*buf = *lexinput ++) ? 1 : (lexinput --, 0); 

char **expression_varNames = NULL;
int    num_expression_varNames = 0;

int
LookupVar(char *name)
{
    int i;

    for(i = 0 ; i < num_expression_varNames && expression_varNames ; i++) {
        if (strcmp(expression_varNames[i], name) == 0)
            return i;
    }

    return -1;
}

%}

digit	[0-9]
exp	[eE][+-]?{digit}+
punct	[.:] 		
letter	[a-zA-Z_]
lead    [_#]
id	{lead}{letter}({letter}|{digit})*

%{
/* punct used to include /, why? */
%}

%x	comment
%%


%{
/* Reserved words (in expression or right hand side of assignment) */
%}

pi				{return CONST_PI;}
sin				{return SIN;}
cos				{return COS;}
tan				{return TAN;}
sinh				{return SINH;}
cosh				{return COSH;}
tanh				{return TANH;}
atan2				{return ATAN2;}
atan				{return ATAN;}
pow				{return POW;}
exp				{return EXP;}
log				{return LOG;}
log10				{return LOG10;}
sqrt				{return SQRT;}
hypot				{return HYPOT;}
floor				{return FLOOR;}
ceil				{return CEIL;}
fmod				{return FMOD;}
fabs				{return FABS;}

%{
/* Operators */
%}

"&&"				{return AND;}
"||"				{return OR;}
"=="				{return EQUALS;}
"!="				{return NEQUAL;}
"<="				{return LT_EQ;}
">="				{return GT_EQ;}
"<<"				{return LSHIFT;}
">>"				{return RSHIFT;}


%{
/* Numbers and names */
%}

{id}				{yylval.s = strdup (yytext); return NAME;}
{digit}+			{yylval.d = atof (yytext); return DOUBLE;}
{digit}+{exp}			{yylval.d = atof (yytext); return DOUBLE;}
{digit}+\.{digit}*{exp}?	{yylval.d = atof (yytext); return DOUBLE;}
{digit}*\.{digit}+{exp}?	{yylval.d = atof (yytext); return DOUBLE;}

\"[^"]*\"			{yylval.s = strdup (yytext + 1);
				 yylval.s [yyleng - 2] = 0; return NAME;}


%{
/* white space */
%}

[ \t]+				{}
.				{return *yytext;}  

%%

static int err = 0;

/************************************************************************
 * Function:	yyerror							*
 *									*
 * Description:	Writes a syntax error message reporting the unexpected	*
 *		token                                          		*
 ************************************************************************/

void yyerror (msg)
    char *msg;
{
    fprintf(stderr,"%s\n", msg);
    err ++;
}


/************************************************************************
 * Function:	init_lexer						*
 *									*
 * Description:	Initializes the lexer for a new file.			*
 ************************************************************************/

Code initLexer (char *line)
{
   err = 0;
   lexinput = line;

   yyparse( );
   if (err)
      return NULL;
   else
      return CopyInCoreCode();
   
}



# undef yywrap
int yywrap ( )
{
    if (0) unput (0);
# if 0
# ifdef YY_SC_TO_UI	/* eliminates warnings with gcc -Wall */
    if (0) {
	yy_push_state (yy_top_state ( ));
	yy_pop_state ( );
	yyunput (0, NULL);
    }
# endif
# endif
    return 1;
}
