/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "ExampleFlexLexer" */
%option prefix="Jasspp"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

%{
#include <iostream>

#include <boost/format.hpp>

#include "driver.hpp"
#include "../../internationalisation.hpp"
%}

LINEDOCCOMMENT  [/][/][/].*\r?\n
PREPROCESSOR    [#].*\r?\n
LINECOMMENT	[/][/].*\r?\n
NEWLINE		\r?\n
REAL       	[0-9]*"."[0-9]*
INTEGER		[0-9]+
BINARY		[0%][0-9]+
OCTAL		[0][0-7]+
HEX		[0x][0-9a-fA-F]+
IDENTIFIER	[a-zA-Z]([a-zA-Z0-9_]*[a-zA-Z0-9])?
SPACE		[ \t]
DOCCOMMENTSTART	[/][*][*]
COMMENTSTART	[/][*]
COMMENTEND	[*][/]
STRINGSTART	["]
STILLSTRINGA	[^"\\]
STILLSTRINGB	[\\][btrn"\\]
STRINGDONE	["]
FSTRINGSTART	["]
FSTILLSTRINGA	[^"\\]
FSTILLSTRINGB	[\\].
FSTRINGDONE	["]
UNITTYPEINT	['][a-zA-Z0-9][a-zA-Z0-9]?[a-zA-Z0-9]?[a-zA-Z0-9]?[']

/* Regulary expressions */
%%

{STRINGSTART}({STILLSTRINGA}|{STILLSTRINGB})*{STRINGDONE} { return StringLiteral; }
{FSTRINGSTART}({FSTILLSTRINGA}|{FSTILLSTRINGB})*{FSTRINGDONE} { printf( "Non valid escape sequences in string\n" ); }
{LINEDOCCOMMENT} { return LineDocComment; }
{PREPROCESSOR} { return Preprocessor; }
{LINECOMMENT} { return LineComment; }
{DOCCOMMENTSTART}.*{COMMENTEND} { return BlockDocComment; }
{DOCCOMMENTSTART}.*{COMMENTEND} { return BlockComment; }
{NEWLINE} { return NewLine; }
{REAL} { return RealLiteral; }
({INTEGER})|({BINARY})|({OCTAL})|({HEX}) { return IntegerLiteral; }
{IDENTIFIER} { return Identifier; }
{UNITTYPEINT} { return UnitTypeIntegerLiteral; }
"true"|"false" return BooleanLiteral;
"null" return NullKeyword;
"type" return TypeKeyword;

[ \t]+ /* eat up whitespace */

/* TODO Add syntax error to corresponding parser? */
. std::cerr << boost::format(_("Unrecognized character: %1%")) % yytext  << std::endl;


%% /*** Additional Code ***/
