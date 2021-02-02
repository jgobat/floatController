%{

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "expressions.h"
# include "eevars.h"

extern void yyerror ( );
extern int  yylex  ( );
extern void EmitCode(Opcode, ...);

static char msg[256];
int         x;
float       varx;

%}

%union {
    int           i;
    double        d;
    char         *s;
}

%right	'?' ':'
%left	OR
%left	AND
%left	'|'
%left	'^'
%left	'&'
%left	EQUALS NEQUAL
%left	'<' '>' LT_EQ GT_EQ
%left	LSHIFT RSHIFT
%left	'+' '-'
%left	'*' '/' '%'
%right	UNARY '!' '~'


%token	NAME DOUBLE
%token  CONST_PI

%token  SIN COS TAN POW EXP LOG LOG10 SQRT HYPOT FLOOR CEIL FMOD FABS
%token  SINH COSH TANH
%token  ATAN ATAN2

%type	<d> DOUBLE 
%type	<s> NAME
%type	<i> expression function or_action and_action if_action else_action 
            
%%

specification
	: variable_expression
	;


/* Expressions */

variable_expression
	: expression
	    {
		EmitCode (HaltOp);
		SetIP (0);
	    }
	;

expression
	: expression '?' if_action expression ':' else_action expression
	    {
		int ip = GetIP ( );
		SetIP (ip - $7 - 2);
		EmitCode (JmpOp, $7);
		SetIP (GetIP ( ) - $4 - 4);
		EmitCode (JzOp, $4 + 2);
		SetIP (ip);
		$$ = $1 + $3 + $4 + $6 + $7;
	    }

	| expression OR or_action expression
	    {
		int ip = GetIP ( );
		SetIP (ip - $4 - 3);
		EmitCode (JnzOp, $4 + 1);
		SetIP (ip);
		EmitCode (TestOp);
		$$ = $1 + $3 + $4 + 1;
	    }

	| expression AND and_action expression
	    {
		int ip = GetIP ( );
		SetIP (ip - $4 - 3);
		EmitCode (JzOp, $4 + 1);
		SetIP (ip);
		EmitCode (TestOp);
		$$ = $1 + $3 + $4 + 1;
	    }

	| expression '|' expression
	    {
		EmitCode (OrOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '^' expression
	    {
		EmitCode (XorOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '&' expression
	    {
		EmitCode (AndOp);
		$$ = $1 + 1 + $3;
	    }

	| expression EQUALS expression
	    {
		EmitCode (EqOp);
		$$ = $1 + 1 + $3;
	    }

	| expression NEQUAL expression
	    {
		EmitCode (NeqOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '<' expression
	    {
		EmitCode (LtOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '>' expression
	    {
		EmitCode (GtOp);
		$$ = $1 + 1 + $3;
	    }

	| expression LT_EQ expression
	    {
		EmitCode (LteqOp);
		$$ = $1 + 1 + $3;
	    }

	| expression GT_EQ expression
	    {
		EmitCode (GteqOp);
		$$ = $1 + 1 + $3;
	    }

	| expression LSHIFT expression
	    {
		EmitCode (LsftOp);
		$$ = $1 + 1 + $3;
	    }

	| expression RSHIFT expression
	    {
		EmitCode (RsftOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '+' expression
	    {
		EmitCode (AddOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '-' expression
	    {
		EmitCode (SubOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '*' expression
	    {
		EmitCode (MulOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '/' expression
	    {
		EmitCode (DivOp);
		$$ = $1 + 1 + $3;
	    }

	| expression '%' expression
	    {
		EmitCode (ModOp);
		$$ = $1 + 1 + $3;
	    }

	| '+' expression		%prec UNARY
	    {
		$$ = $2;
	    }

	| '-' expression		%prec UNARY
	    {
		EmitCode (NegOp);
		$$ = 1 + $2;
	    }

	| '!' expression
	    {
		EmitCode (NotOp);
		$$ = 1 + $2;
	    }

	| '~' expression
	    {
		EmitCode (InvOp);
		$$ = 1 + $2;
	    }

	| '(' expression ')'
	    {
		$$ = $2;
	    }

	| DOUBLE
	    {
		    EmitCode (PushOp, $1);
		    $$ = 2;
	    }

        | CONST_PI
	    {
		    EmitCode (PushOp, M_PI);
	        $$ = 2;
	    }

	| NAME
	    {
            if ($1[0] == '_') {
                varx = eeGetFloat($1);
                if (!isnan(varx))
                    EmitCode(PushOp, varx);
                else {
                    sprintf(msg, "invalid column reference %s in expression", $1);
                    yyerror(msg);
                }
            }
            else if ($1[0] == '#') {
            }
	        $$ = 2;
	    }

	| function
	;


function
	: SIN '(' expression ')'
	    {
		EmitCode (SinOp);
		$$ = $3 + 1;
	    }

	| COS '(' expression ')'
	    {
		EmitCode (CosOp);
		$$ = $3 + 1;
	    }

	| TAN '(' expression ')'
	    {
		EmitCode (TanOp);
		$$ = $3 + 1;
	    }

	| ATAN2 '(' expression ',' expression ')'
	    {
		EmitCode (Atan2Op);
		$$ = $3 + $5 + 1;
	    }

	| ATAN '(' expression ')'
	    {
		EmitCode (AtanOp);
		$$ = $3 + 1;
	    }


	| TANH '(' expression ')'
	    {
		EmitCode (TanhOp);
		$$ = $3 + 1;
	    }

	| SINH '(' expression ')'
	    {
		EmitCode (SinhOp);
		$$ = $3 + 1;
	    }

	| COSH '(' expression ')'
	    {
		EmitCode (CoshOp);
		$$ = $3 + 1;
	    }

	| POW '(' expression ',' expression ')'
	    {
		EmitCode (PowOp);
		$$ = $3 + $5 + 1;
	    }

	| EXP '(' expression ')'
	    {
		EmitCode (ExpOp);
		$$ = $3 + 1;
	    }

	| LOG '(' expression ')'
	    {
		EmitCode (LnOp);
		$$ = $3 + 1;
	    }

	| LOG10 '(' expression ')'
	    {
		EmitCode (LogOp);
		$$ = $3 + 1;
	    }

	| SQRT '(' expression ')'
	    {
		EmitCode (SqrtOp);
		$$ = $3 + 1;
	    }

	| HYPOT '(' expression ',' expression ')'
	    {
		EmitCode (HypotOp);
		$$ = $3 + $5 + 1;
	    }

	| FLOOR '(' expression ')'
	    {
		EmitCode (FloorOp);
		$$ = $3 + 1;
	    }

	| CEIL '(' expression ')'
	    {
		EmitCode (CeilOp);
		$$ = $3 + 1;
	    }

	| FMOD '(' expression ',' expression ')'
	    {
		EmitCode (FmodOp);
		$$ = $3 + $5 + 1;
	    }

	| FABS '(' expression ')'
	    {
		EmitCode (FabsOp);
		$$ = $3 + 1;
	    }

	;


if_action
	: /* empty */
	    {
		EmitCode (JzOp, 0);
		$$ = 2;
	    }
	;


else_action
	: /* empty */
	    {
		EmitCode (JmpOp, 0);
		$$ = 2;
	    }
	;


or_action
	: /* empty */
	    {
		EmitCode (CopyOp);
		EmitCode (JnzOp, 0);
		EmitCode (PopOp);
		$$ = 4;
	    }
	;


and_action
	: /* empty */
	    {
		EmitCode (CopyOp);
		EmitCode (JzOp, 0);
		EmitCode (PopOp);
		$$ = 4;
	    }
	;

%%
