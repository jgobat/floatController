/************************************************************************
 * File:	code.c							*
 *									*
 * Description:	This file contains the public and private function and	*
 *		type definitions for the stack machine code.		*
 ************************************************************************/

# include <stdio.h>
# include <math.h>
# include <stdarg.h>
# include <stdlib.h>
# include "expressions.h"


# define MaxStackDepth	2048
# define MaxCodeSize	2048

# define push(x)	(* ++ sp = (x))
# define pop()		(* sp --)
# define top()		(* sp)

# define deg2rad(x)	((x) * M_PI / 180.0)


# define None	 0
# define Integer 1
# define Double  2

static struct {
    char *opcode;
    int   arg_type;
} data [ ] = {
    {"jmp",   Integer},
    {"jnz",   Integer},
    {"jz",    Integer},
    {"push",  Double},
    {"pop",   None},
    {"copy",  None},
    {"test",  None},
    {"neg",   None},
    {"not",   None},
    {"inv",   None},
    {"mul",   None},
    {"div",   None},
    {"mod",   None},
    {"add",   None},
    {"sub",   None},
    {"lsft",  None},
    {"rsht",  None},
    {"lt",    None},
    {"gt",    None},
    {"lteq",  None},
    {"gteq",  None},
    {"eq",    None},
    {"neq",   None},
    {"and",   None},
    {"xor",   None},
    {"or",    None},
    {"sin",   None},
    {"cos",   None},
    {"tan",   None},
    {"sinh",  None},
    {"cosh",  None},
    {"tanh",  None},
    {"atan",  None},
    {"atan2", None},
    {"exp",   None},
    {"ln",    None},
    {"log",   None},
    {"pow",   None},
    {"sqrt",  None},
    {"hypot", None},
    {"floor", None},
    {"ceil",  None},
    {"fmod",  None},
    {"fabs",  None},
    {"halt",  None},
};


typedef union instruction {
    Opcode  op;
    int     offset;
    double  arg;
    int     colidx;
} Instruction;


static Instruction in_core [MaxCodeSize];
static Code	   ip = in_core;


Code InCore = in_core;


/************************************************************************
 * Function:	EmitCode						*
 *									*
 * Description:	Adds an instruction to the current piece of code.	*
 ************************************************************************/

void 
EmitCode (Opcode op, ...)
{
    va_list ap;

    ip ++ -> op = op;
    
    switch (data [op].arg_type) {
    case Integer:
	    va_start (ap, op);
	    ip ++ -> offset = va_arg (ap, int);
	    va_end (ap);
	    break;

    case Double:
	    va_start (ap, op);
	    ip ++ -> arg = va_arg (ap, double);
	    va_end (ap);
	    break;
	}
}

/************************************************************************
 * Function:	CopyCode						*
 *									*
 * Description:	Copies a piece of code.					*
 ************************************************************************/

Code 
CopyCode (Code code)
{
    Code     pc;
    Code     ptr;
    Code     copy;
    Opcode   op;
    unsigned size;


    size = 0;
    if (!(pc = code))
	    return NULL;

    while (1) {
	    size ++;
	    if ((op = pc ++ -> op) == HaltOp)
	        break;

	    if (data [op].arg_type != None) {
	        size ++;
	        pc ++;
	    }
    }

    if (!(copy = (Instruction *) malloc(sizeof(Instruction) * size)))
	    return NULL;

    pc = code;
    ptr = copy;

    while (size --)
	*ptr ++ = *pc ++;

    return copy;
}

Code 
CopyInCoreCode(void)
{
   return CopyCode(InCore);
}

/************************************************************************
 * Function:	FreeCode						*
 *									*
 * Description:	Deallocates a copied program.				*
 ************************************************************************/

void 
FreeCode (Code code)
{
    if (code != InCore)
	    free(code);
}


/************************************************************************
 * Function:	SetIP							*
 *									*
 * Description:	Sets the address of the instruction pointer.		*
 ************************************************************************/

void 
SetIP (int new_ip)
{
    ip = InCore + new_ip;
}


/************************************************************************
 * Function:	GetIP							*
 *									*
 * Description:	Returns the address of the instruction pointer.		*
 ************************************************************************/

int 
GetIP(void)
{
    return ip - InCore;
}



/************************************************************************
 * Function:	EvalCode						*
 *									*
 * Description:	Evaluates a piece of code.				*
 ************************************************************************/

double
EvalCode (Code code)
{
    int     x;
    int     y;
    double  a, b, c, d;
    Code    pc;
    int     idx;
    double	   stack [MaxStackDepth];
    double	  *sp;

    sp = stack;
    if (!(pc = code))
	return 0;

    while (1)
	switch (pc ++ -> op) {
	case JmpOp:
	    y = pc ++ -> offset;
	    pc += y;
	    break;

	case JnzOp:
	    x = pop ( );
	    y = pc ++ -> offset;
	    if (x) pc += y;
	    break;

	case JzOp:
	    x = pop ( );
	    y = pc ++ -> offset;
	    if (!x) pc += y;
	    break;

	case PushOp:
	    push (pc ++ -> arg);
	    break;

	case PopOp:
	    a = pop ( );
	    break;

	case CopyOp:
	    a = top ( );
	    push (a);
	    break;

	case TestOp:
	    a = pop ( );
	    push (a != 0);
	    break;

	case NegOp:
	    a = pop ( );
	    push (-a);
	    break;

	case NotOp:
	    a = pop ( );
	    push (!a);
	    break;

	case InvOp:
	    x = pop ( );
	    push (~x);
	    break;

	case MulOp:
	    b = pop ( );
	    a = pop ( );
	    push (a * b);
	    break;

	case DivOp:
	    b = pop ( );
	    a = pop ( );
	    push (b ? a / b : 0);
	    break;

	case ModOp:
	    y = pop ( );
	    x = pop ( );
	    push (y ? x % y : 0);
	    break;

	case AddOp:
	    b = pop ( );
	    a = pop ( );
	    push (a + b);
	    break;

	case SubOp:
	    b = pop ( );
	    a = pop ( );
	    push (a - b);
	    break;

	case LsftOp:
	    y = pop ( );
	    x = pop ( );
	    push (x << y);
	    break;

	case RsftOp:
	    y = pop ( );
	    x = pop ( );
	    push (x >> y);
	    break;

	case LtOp:
	    b = pop ( );
	    a = pop ( );
	    push (a < b);
	    break;

	case GtOp:
	    b = pop ( );
	    a = pop ( );
	    push (a > b);
	    break;

	case LteqOp:
	    b = pop ( );
	    a = pop ( );
	    push (a <= b);
	    break;

	case GteqOp:
	    b = pop ( );
	    a = pop ( );
	    push (a >= b);
	    break;

	case EqOp:
	    b = pop ( );
	    a = pop ( );
	    push (a == b);
	    break;

	case NeqOp:
	    b = pop ( );
	    a = pop ( );
	    push (a != b);
	    break;

	case AndOp:
	    y = pop ( );
	    x = pop ( );
	    push (x & y);
	    break;

	case XorOp:
	    y = pop ( );
	    x = pop ( );
	    push (x ^ y);
	    break;

	case OrOp:
	    y = pop ( );
	    x = pop ( );
	    push (x | y);
	    break;

	case SinOp:
	    a = pop ( );
	    push (sin (a));
	    break;

	case CosOp:
	    a = pop ( );
	    push (cos (a));
	    break;

	case TanOp:
	    a = pop ( );
	    push (tan (a));
	    break;

	case AtanOp:
	    a = pop ( );
	    push (atan (a));
	    break;

	case Atan2Op:
	    b = pop ( );
	    a = pop ( );
	    push (atan2 (a, b));
	    break;

	case SinhOp:
	    a = pop ( );
	    push (sinh (a));
	    break;

	case CoshOp:
	    a = pop ( );
	    push (cosh (a));
	    break;

	case TanhOp:
	    a = pop ( );
	    push (tanh (a));
	    break;

	case ExpOp:
	    a = pop ( );
	    push (exp (a));
	    break;

	case LnOp:
	    a = pop ( );
	    push (a > 0 ? log (a) : 0);
	    break;

	case LogOp:
	    a = pop ( );
	    push (a > 0 ? log10 (a) : 0);
	    break;

	case PowOp:
	    b = pop ( );
	    a = pop ( );
	    push (a >= 0 || b == (int) b ? pow (a, b) : 0);
	    break;

	case SqrtOp:
	    a = pop ( );
	    push (a >= 0 ? sqrt (a) : 0);
	    break;

	case HypotOp:
	    b = pop ( );
	    a = pop ( );
	    push (hypot (a, b));
	    break;

	case FloorOp:
	    a = pop ( );
	    push (floor (a));
	    break;

	case CeilOp:
	    a = pop ( );
	    push (ceil (a));
	    break;

	case FmodOp:
	    b = pop ( );
	    a = pop ( );
	    push (b ? fmod (a, b) : 0);
	    break;

	case FabsOp:
	    a = pop ( );
	    push (fabs (a));
	    break;

	case HaltOp:
	    return pop ( );
	}
}


/************************************************************************
 * Function:	DebugCode						*
 *									*
 * Description:	Print a piece of stack code as instructions.		*
 ************************************************************************/

void 
DebugCode (Code code)
{
    int     x;
    Opcode  op;
    Code    pc;


    if (!(pc = code))
	return;

    while (1) {
	op = pc -> op;
	printf ("%lx\t%s", (long) (pc ++), data [op].opcode);

	switch (data [op].arg_type) {
	case Integer:
	    x = pc ++ -> offset;
	    printf ("\t%lx\n", (long) (pc + x));
	    break;

	case Double:
	    printf ("\t%g\n", pc ++ -> arg);
	    break;

	default:
	    printf ("\n");
	}

	if (op == HaltOp)
	    return;
    }
}


/************************************************************************
 * Function:	IsConstant						*
 *									*
 * Description:	Determines if a piece of code is constant.		*
 ************************************************************************/

int 
IsConstant (Code code)
{
    Opcode op;
    Code   pc;

    op = 0;	/* gcc -Wall */

    if (!(pc = code))
	return 1;

    while (1)
	switch (op = pc ++ -> op) {

	case HaltOp:
	    return 1;

	default:
	    if (data [op].arg_type != None)
		pc ++;
    }
}
