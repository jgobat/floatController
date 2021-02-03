/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    OR = 258,
    AND = 259,
    EQUALS = 260,
    NEQUAL = 261,
    LT_EQ = 262,
    GT_EQ = 263,
    LSHIFT = 264,
    RSHIFT = 265,
    UNARY = 266,
    NAME = 267,
    DOUBLE = 268,
    CONST_PI = 269,
    SIN = 270,
    COS = 271,
    TAN = 272,
    POW = 273,
    EXP = 274,
    LOG = 275,
    LOG10 = 276,
    SQRT = 277,
    HYPOT = 278,
    FLOOR = 279,
    CEIL = 280,
    FMOD = 281,
    FABS = 282,
    SINH = 283,
    COSH = 284,
    TANH = 285,
    ATAN = 286,
    ATAN2 = 287
  };
#endif
/* Tokens.  */
#define OR 258
#define AND 259
#define EQUALS 260
#define NEQUAL 261
#define LT_EQ 262
#define GT_EQ 263
#define LSHIFT 264
#define RSHIFT 265
#define UNARY 266
#define NAME 267
#define DOUBLE 268
#define CONST_PI 269
#define SIN 270
#define COS 271
#define TAN 272
#define POW 273
#define EXP 274
#define LOG 275
#define LOG10 276
#define SQRT 277
#define HYPOT 278
#define FLOOR 279
#define CEIL 280
#define FMOD 281
#define FABS 282
#define SINH 283
#define COSH 284
#define TANH 285
#define ATAN 286
#define ATAN2 287

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 21 "parser.y"

    int           i;
    double        d;
    char         *s;

#line 127 "parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
