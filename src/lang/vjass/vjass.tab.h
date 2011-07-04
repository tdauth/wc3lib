
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LDC = 258,
     PP = 259,
     LC = 260,
     NEWLINE = 261,
     DIGIT = 262,
     HEXDIGIT = 263,
     ID = 264,
     SPACE = 265,
     DCSTART = 266,
     CSTART = 267,
     CEND = 268,
     STRINGSTART = 269,
     STILLSTRINGA = 270,
     STILLSTRINGB = 271,
     STRINGDONE = 272,
     FSTRINGSTART = 273,
     FSTILLSTRINGA = 274,
     FSTILLSTRINGB = 275,
     FSTRINGDONE = 276,
     UNITTYPEINT = 277,
     LineDocComment = 278,
     LineComment = 279,
     BlockComment = 280,
     BlockDocComment = 281,
     NewLine = 282,
     Identifier = 283,
     UnitTypeIntegerLiteral = 284,
     IntegerLiteral = 285,
     BooleanLiteral = 286,
     RealLiteral = 287,
     StringLiteral = 288,
     NullKeyword = 289,
     TypeKeyword = 290,
     ConstantKeyword = 291,
     NativeKeyword = 292,
     FunctionKeyword = 293,
     EndfunctionKeyword = 294,
     GlobalsKeyword = 295,
     EndglobalsKeyword = 296,
     LocalKeyword = 297,
     SetKeyword = 298,
     CallKeyword = 299,
     IfKeyword = 300,
     ThenKeyword = 301,
     ElseKeyword = 302,
     ElseifKeyword = 303,
     EndifKeyword = 304,
     ReturnKeyword = 305,
     LoopKeyword = 306,
     EndloopKeyword = 307,
     ExitwhenKeyword = 308,
     NothingKeyword = 309,
     TakesKeyword = 310,
     ReturnsKeyword = 311,
     ExtendsKeyword = 312,
     ArrayKeyword = 313,
     AndKeyword = 314,
     OrKeyword = 315,
     Dot = 316,
     Comma = 317,
     LeftParenthesis = 318,
     RightParenthesis = 319,
     LeftSquaredBracked = 320,
     RightSquaredBracked = 321,
     SetOperator = 322,
     MultiplicationOperator = 323,
     DivisionOperator = 324,
     PlusOperator = 325,
     MinusOperator = 326,
     LessOperator = 327,
     GreaterOperator = 328,
     EqualOperator = 329,
     LessEqualOperator = 330,
     GreaterEqualOperator = 331,
     NotEqualOperator = 332,
     VjassPreprocessor = 333,
     JassppPreprocessor = 334,
     MethodKeyword = 335,
     EndmethodKeyword = 336,
     PrivateKeyword = 337,
     PublicKeyword = 338,
     StaticKeyword = 339,
     DebugKeyword = 340,
     KeywordKeyword = 341,
     LibraryKeyword = 342,
     Library_onceKeyword = 343,
     EndlibraryKeyword = 344,
     ScopeKeyword = 345,
     EndscopeKeyword = 346,
     StructKeyword = 347,
     EndstructKeyword = 348,
     InterfaceKeyword = 349,
     EndinterfaceKeyword = 350,
     DelegateKeyword = 351,
     StubKeyword = 352,
     ModuleKeyword = 353,
     EndmoduleKeyword = 354,
     ImplementKeyword = 355,
     HookKeyword = 356,
     KeyKeyword = 357,
     ImportKeyword = 358,
     DovjassinitKeyword = 359,
     InjectKeyword = 360,
     EndinjectKeyword = 361,
     NovjassKeyword = 362,
     EndnovjassKeyword = 363,
     VjassKeyword = 364,
     EndvjassKeyword = 365,
     ZincKeyword = 366,
     EndzincKeyword = 367,
     ExternalblockKeyword = 368,
     EndexternalblockKeyword = 369,
     LoaddataKeyword = 370,
     ExternalKeyword = 371,
     TextmacroKeyword = 372,
     TextmacroonceKeyword = 373,
     EndtextmacroKeyword = 374,
     RuntextmacroKeyword = 375,
     RequiresKeyword = 376,
     NeedsKeyword = 377,
     UsesKeyword = 378,
     InitializerKeyword = 379,
     DefaultsKeyword = 380,
     OptionalKeyword = 381,
     SuperKeyword = 382,
     ThistypeKeyword = 383,
     OperatorKeyword = 384,
     ExecuteKeyword = 385,
     EvaluateKeyword = 386,
     GlobalKeyword = 387
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


