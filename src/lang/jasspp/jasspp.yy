
%{
%}

/* Require bison 2.4 or later */
%require "2.4"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "StaticScopeContent" */
%start StaticScopeContent

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="wc3lib"

/* set the parser's class identifier */
%define "parser_class_name" "Parser"

/* keep track of the current position within the input */
%locations

%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }

/* verbose error messages */
%error-verbose

// GRAMMAR DEFINITIONS START HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
%union {
    class wc3lib::lang::Token *token;
}

%token END "End of file"
%token EOL "End of line"
%token LIT_NULLVALUE
%token LIT_INTEGER
%token LIT_REAL
%token LIT_STRING
%token LIT_BOOLEAN
%token IDENTIFIER

%token LINE_COMMENT
%token BLOCK_COMMENT

// keywords
%token STATIC
%token DYNAMIC
%token VAR
%token CONST
%token IF
%token ELSE
%token SWITCH
%token CASE
%token DEFAULT
%token TRY
%token CATCH
%token THROW
%token HANDLE
%token CODE
%token BOOLEAN
%token INTEGER
%token REAL
%token STRING

// preprocessor keywords
%token P_INCLUDE
%token P_IF
%token P_ELIF
%token P_ELSE
%token P_ENDIF

//%token PP

%{

#include "driver.hpp"
#include "scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}


%%

/// @todo Move preprocessor to scanner (Flex) and do only parse files wich are necessary.
PreprocessorStatement
	: P_INCLUDE PreprocessorFilePath
	| P_IF PreprocessorExpression
	| P_ELIF PreprocessorExpression
	| P_ELSE
	| P_ENDIF
	;

Preprocessor
	: PREPROCESSOR PreprocessorStatement
	;

LineComment
	: LINE_COMMENT
	;

BlockComment
	: BLOCK_COMMENT
	;

LineDocComment
	: '/' LineComment
	;

BlockDocComment
	: '/' BlockComment
	;

DocComment
	: LineDocComment
	| BlockDocComment
	{
	}
	;

Comment
	: LineComment
	| BlockComment
	| LineDocComment
	| BlockDocComment
	;

Literal : LIT_NULLVALUE
	| LIT_INTEGER
	| LIT_REAL
	| LIT_STRING
	| LIT_BOOLEAN
	;

Identifier : IDENTIFIER
	;

Type
	: HANDLE
	| CODE
	| BOOLEAN
	| INTEGER
	| REAL
	| STRING
	;

VariableType
	: Type
	| Type CONST
	;

Expression : Literal Operator Expression
	| Identifier Operator Expression
	| Literal
	| Identifier
	| '(' Literal Operator Expression ')'
	| '(' Identifier Operator Expression ')'
	| '(' Literal ')'
	| '(' Identifier ')'
	;

ContainerExpression
	: Expression ',' ContainerExpression
	| Expression
	;

ContainerAssignment : '=' '{' ContainerExpression '}'
	;

Assignment : '=' Expression
	| ContainerAssignment
	;

DynamicVariableDeclarationPrefix
	: DYNAMIC
	| DYNAMIC VAR
	| VAR
	;

StaticVariableDeclarationPrefix
	: STATIC
	| STATIC VAR
	| VAR
	;

VariableAssignment
	: Identifier Assignment
	| Identifier
	;

VariablesAssignments
	: VariableAssignment ',' VariablesAssignments
	| VariableAssignment
	;

DynamicVariableDeclaration
	: DynamicVariableDeclarationPrefix VariableType VariablesAssignments ';'
	;

StaticVariableDeclaration
	: StaticVariableDeclarationPrefix VariableType VariablesAssignments ';'
	;

StaticDeclarations
	: StaticVariableDeclaration StaticDeclarations
	| Comment StaticDeclarations
	;

// Start symbol!
StaticScopeContent
	: StaticDeclarations
	;

Statement
	: AssignmentStatement
	| IfStatement
	| SwitchStatement
	| FinallyStatement
	| TryStatement
	| BREAK ';'
	| CONTINUE ';'
	;

Statements
	: Statement Statements
	| Statement
	;

AssignmentStatement
	: Identifier Assignment ';'
	;

/* Dynamic-only declarations go here */
StatementExpression
	: '(' Expression ')'
	;

StatementBlock
	: '{' Statements '}'
	;

IfStatement
	: IF StatementExpression StatementBlock
	| IF StatementExpression StatementBlock IfStatementElses
	;

IfStatementElses
	: ELSE IF StatementExpression StatementBlock
	| ELSE IF StatementExpression StatementBlock IfStatementElses
	| ELSE StatementBlock
	;

SwitchStatementBlock
	: CASE StatementExpression StatementBlock
	| DEFAULT StatementBlock
	;

SwitchStatement
	: SWITCH StatementExpression SwitchStatementBlock
	;

FinallyStatement
	: FINALLY StatementBlock
	;

ThrowStatement
	: THROW Expression ';'
	;

TryStatementCatch
	: CATCH StatementExpression StatementBlock
	| DEFAULT StatementBlock 
	;

TryStatementCatches
	: TryStatementCatch TryStatementCatches
	| TryStatementCatch
	;

TryStatement
	: TRY StatementBlock
	| TRY StatementBlock TryStatementCatches
	;

DynamicDeclarations
	: DynamicVariableDeclaration DynamicDeclarations
	| Comment DynamicDeclarations
	;

DynamicScopeContent
	: DynamicDeclarations
	;

%%
