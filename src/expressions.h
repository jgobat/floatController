/************************************************************************
 * File:	code.h							*
 *									*
 * Description:	This file contains the public function and type		*
 *		declarations for the stack machine code.		*
 ************************************************************************/

# ifndef _CODE_H
# define _CODE_H

# include <stdarg.h>

typedef union instruction *Code;

typedef enum {
    JmpOp,		/* unconditional jump	    */
    JnzOp,		/* jump if not zero	    */
    JzOp,		/* jump if zero		    */
    PushOp,		/* push value		    */
    PopOp,		/* pop top of stack	    */
    CopyOp,		/* copy top of stack	    */
    TestOp,		/* test top of stack	    */
    NegOp,		/* unary negation	    */
    NotOp,		/* logical negation	    */
    InvOp,		/* bitwise negation	    */
    MulOp,		/* multiplication	    */
    DivOp,		/* division		    */
    ModOp,		/* modulo		    */
    AddOp,		/* addition		    */
    SubOp,		/* subtraction		    */
    LsftOp,		/* left shift		    */
    RsftOp,		/* right shift		    */
    LtOp,		/* less than		    */
    GtOp,		/* greater than		    */
    LteqOp,		/* less than or equal	    */
    GteqOp,		/* greater than or equal    */
    EqOp,		/* equality		    */
    NeqOp,		/* inequality		    */
    AndOp,		/* bitwise and		    */
    XorOp,		/* bitwise xor		    */
    OrOp,		/* bitwise or		    */
    SinOp,		/* sin function		    */
    CosOp,		/* cos function		    */
    TanOp,		/* tan function		    */
    SinhOp,		/* sinh function	    */
    CoshOp,		/* cosh function	    */
    TanhOp,		/* tanh function	    */
    AtanOp,		/* inverse tan function	    */
    Atan2Op,		/* better inv. tan function */
    ExpOp,		/* exp function		    */
    LnOp,		/* log function		    */
    LogOp,		/* log10 function	    */
    PowOp,		/* pow function		    */
    SqrtOp,		/* sqrt function	    */
    HypotOp,		/* hypot function	    */
    FloorOp,		/* floor function	    */
    CeilOp,		/* ceil function	    */
    FmodOp,		/* fmod function	    */
    FabsOp,		/* fabs function	    */
    HaltOp		/* halt execution	    */
} Opcode;


extern Code   InCore;
extern void   EmitCode(Opcode, ...);
extern Code   CopyCode(Code);
extern Code   CopyInCoreCode( void );
extern void   FreeCode(Code);
extern double EvalCode(Code);
extern void   DebugCode(Code);
extern int    CompileCode(char *);
extern int    IsConstant(Code);
extern void   SetIP(int);
extern int    GetIP(void);

extern Code initLexer(char *);

# endif /* _CODE_H */
