/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_LC3PARSER_TAB_H_INCLUDED
# define YY_YY_LC3PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ADD = 258,                     /* ADD  */
    AND = 259,                     /* AND  */
    OPCODE3I = 260,                /* OPCODE3I  */
    OPCODE2R = 261,                /* OPCODE2R  */
    OPCODE2I = 262,                /* OPCODE2I  */
    OPCODE1R = 263,                /* OPCODE1R  */
    OPCODE1I = 264,                /* OPCODE1I  */
    REGISTER = 265,                /* REGISTER  */
    IMMED = 266,                   /* IMMED  */
    HALT = 267,                    /* HALT  */
    TRAPINT = 268,                 /* TRAPINT  */
    BR = 269,                      /* BR  */
    LABEL = 270,                   /* LABEL  */
    STRING = 271,                  /* STRING  */
    UTSTRING = 272,                /* UTSTRING  */
    INTASSIGN = 273,               /* INTASSIGN  */
    STRASSIGN = 274,               /* STRASSIGN  */
    SPACE = 275,                   /* SPACE  */
    OP_SEP = 276,                  /* OP_SEP  */
    COMMENT = 277,                 /* COMMENT  */
    EMPTYLINE = 278,               /* EMPTYLINE  */
    ENDLINE = 279,                 /* ENDLINE  */
    START = 280,                   /* START  */
    FINISH = 281                   /* FINISH  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 46 "lc3parser.y"

    int16_t number;
    char *string;
    node_t* instrnode;
    uint16_t instruction;
    uint16_t* declaration;

#line 98 "lc3parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_LC3PARSER_TAB_H_INCLUDED  */
