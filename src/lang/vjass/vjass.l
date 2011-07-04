%{

#include <stdio.h>
#include "vjass.tab.h"
 
%}

/* line documentation comment */
LDC             [/][/][/].*\r?\n
/* vJass Preprocessor */
VPP             [/][/][!].*\r?\n
/* Jass++ Preprocessor */
JPP             [#].*\r?\n
/* line comment */
LC		[/][/].*\r?\n
/* new!!! */
NEWLINE		\r?\n
REALDIGIT       [0-9]*"."[0-9]*
DIGIT		[0-9]+
BINARYDIGIT	[0%][0-9]+
OCTALDIGIT	[0][0-7]+
HEXDIGIT	[0x][0-9a-fA-F]+
ID	        [a-zA-Z]([a-zA-Z0-9_]*[a-zA-Z0-9])?
SPACE		[ \t]
DCSTART		[/][*][*]
CSTART		[/][*]
CEND		[*][/]
STRINGSTART	["]
STILLSTRINGA	[^"\\]
STILLSTRINGB	[\\][btrn"\\]
STRINGDONE	["]
FSTRINGSTART	["]
FSTILLSTRINGA	[^"\\]
FSTILLSTRINGB	[\\].
FSTRINGDONE	["]
UNITTYPEINT	['][a-zA-Z0-9][a-zA-Z0-9]?[a-zA-Z0-9]?[a-zA-Z0-9]?[']

%%

{STRINGSTART}({STILLSTRINGA}|{STILLSTRINGB})*{STRINGDONE} { return StringLiteral; }
{FSTRINGSTART}({FSTILLSTRINGA}|{FSTILLSTRINGB})*{FSTRINGDONE} { printf( "Non valid escape sequences in string\n" ); }
{LDC} { return LineDocComment; }
{VPP} { return VjassPreprocessor; }
{JPP} { return JassppPreprocessor; }
{LC} { return LineComment; }
{DCSTART}.*{CEND} { return BlockDocComment; }
{CSTART}.*{CEND} { return BlockComment; }
{NEWLINE}			{ return NewLine; }

"true"|"false"			return BooleanLiteral;

"null"				return NullKeyword;
"type"				return TypeKeyword;
"constant"			return ConstantKeyword;
"native"			return NativeKeyword;
"function"			return FunctionKeyword;
"endfunction"			return EndfunctionKeyword;
"globals"			return GlobalsKeyword;
"endglobals"			return EndglobalsKeyword;
"local"				return LocalKeyword;
"set"				return SetKeyword;
"call"				return CallKeyword;
"if"				return IfKeyword;
"then"				return ThenKeyword;
"else"				return ElseKeyword;
"elseif"			return ElseifKeyword;
"endif"				return EndifKeyword;
"return"			return ReturnKeyword;
"loop"				return LoopKeyword;
"endloop"			return EndloopKeyword;
"exitwhen"			return ExitwhenKeyword;
"nothing"			return NothingKeyword;
"takes"				return TakesKeyword;
"returns"			return ReturnsKeyword;
"extends"			return ExtendsKeyword;
"array"				return ArrayKeyword;
"and"				return AndKeyword;
"or"				return OrKeyword;
"."				return Dot;
","				return Comma;
"("				return LeftParenthesis;
")"				return RightParenthesis;
"["				return LeftSquaredBracked;
"]"				return RightSquaredBracked;
"="				return SetOperator;
"*"				return MultiplicationOperator;
"/"				return DivisionOperator;
"+"				return PlusOperator;
"-"				return MinusOperator;
"<"				return LessOperator;
">"				return GreaterOperator;
"=="				return EqualOperator;
"<="				return LessEqualOperator;
">="				return GreaterEqualOperator;
"!="				return NotEqualOperator;
"//!"				return Preprocessor;
"method"			return MethodKeyword;
"endmethod"			return EndmethodKeyword;
"private"			return PrivateKeyword;
"public"			return PublicKeyword;
"static"			return StaticKeyword;
"debug"				return DebugKeyword;
"keyword"			return KeywordKeyword;
"library"			return LibraryKeyword;
"library_once"			return Library_onceKeyword;
"endlibrary"			return EndlibraryKeyword;
"scope"				return ScopeKeyword;
"endscope"			return EndscopeKeyword;
"struct"			return StructKeyword;
"endstruct"			return EndstructKeyword;
"interface"			return InterfaceKeyword;
"endinterface"			return EndinterfaceKeyword;
"delegate"			return DelegateKeyword;
"stub"				return StubKeyword;
"module"			return ModuleKeyword;
"endmodule"			return EndmoduleKeyword;
"implement"			return ImplementKeyword;
"hook"				return HookKeyword;
"key"				return KeyKeyword;
"import"			return ImportKeyword;
"dovjassinit"			return DovjassinitKeyword;
"inject"			return InjectKeyword;
"endinject"			return EndinjectKeyword;
"novjass"			return NovjassKeyword;
"endnovjass"			return EndnovjassKeyword;
"vjass"				return VjassKeyword;
"endvjass"			return EndvjassKeyword;
"zinc"				return ZincKeyword;
"endzinc"			return EndzincKeyword;
"externalblock"			return ExternalblockKeyword;
"endexternalblock"		return EndexternalblockKeyword;
"loaddata"			return LoaddataKeyword;
"external"			return ExternalKeyword;
"textmacro"			return TextmacroKeyword;
"textmacro_once"		return TextmacroonceKeyword;
"endtextmacro"			return EndtextmacroKeyword;
"runtextmacro"			return RuntextmacroKeyword;
"requires"			return RequiresKeyword;
"needs"				return NeedsKeyword;
"uses"				return UsesKeyword;
"initializer"			return InitializerKeyword;
"defaults"			return DefaultsKeyword;
"optional"			return OptionalKeyword;
"super"				return SuperKeyword;
"thistype"			return ThistypeKeyword;
"operator"			return OperatorKeyword;
"execute"			return ExecuteKeyword;
"evaluate"			return EvaluateKeyword;
"global"			return GlobalKeyword;

{REALDIGIT} { return RealLiteral; }

({DIGIT})|({BINARYDIGIT})|({OCTALDIGIT})|({HEXDIGIT}) { return IntegerLiteral; }

{ID} { return Identifier; }
{UNITTYPEINT} { return UnitTypeIntegerLiteral; }

[ \t]+          /* eat up whitespace */

. printf( "Unrecognized character: %s\n", yytext );

%%
