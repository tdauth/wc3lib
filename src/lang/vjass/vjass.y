%{

#include "vjass.hpp"
 
using namespace wc3lib::lang;
 
class Parser *parser = 0;
 
void yyerror(const char *str)
{
	parser->addSyntaxError(str);
}
 
int yywrap()
{
	// open other files, this is called when file ends, return 1 if it's the end
        return 1;
} 

/*  
main()
{
        yyparse();
}
*/

%}

%token LDC
%token PP
%token LC
%token NEWLINE
%token DIGIT
%token HEXDIGIT
%token ID
%token SPACE
%token DCSTART
%token CSTART
%token CEND
%token STRINGSTART
%token STILLSTRINGA
%token STILLSTRINGB
%token STRINGDONE
%token FSTRINGSTART
%token FSTILLSTRINGA
%token FSTILLSTRINGB
%token FSTRINGDONE
%token UNITTYPEINT

%token LineDocComment
%token LineComment
%token BlockComment
%token BlockDocComment
%token NewLine
%token Identifier
%token UnitTypeIntegerLiteral
%token IntegerLiteral
%token BooleanLiteral
%token RealLiteral
%token StringLiteral
%token NullKeyword
%token TypeKeyword
%token ConstantKeyword
%token NativeKeyword
%token FunctionKeyword
%token EndfunctionKeyword
%token GlobalsKeyword
%token EndglobalsKeyword
%token LocalKeyword
%token SetKeyword
%token CallKeyword
%token IfKeyword
%token ThenKeyword
%token ElseKeyword
%token ElseifKeyword
%token EndifKeyword
%token ReturnKeyword
%token LoopKeyword;
%token EndloopKeyword
%token ExitwhenKeyword
%token NothingKeyword
%token TakesKeyword
%token ReturnsKeyword
%token ExtendsKeyword
%token ArrayKeyword
%token AndKeyword
%token OrKeyword
%token Dot
%token Comma
%token LeftParenthesis
%token RightParenthesis
%token LeftSquaredBracked
%token RightSquaredBracked
%token SetOperator
%token MultiplicationOperator
%token DivisionOperator
%token PlusOperator
%token MinusOperator
%token LessOperator
%token GreaterOperator
%token EqualOperator
%token LessEqualOperator
%token GreaterEqualOperator
%token NotEqualOperator
%token VjassPreprocessor
%token JassppPreprocessor
%token MethodKeyword
%token EndmethodKeyword
%token PrivateKeyword
%token PublicKeyword
%token StaticKeyword
%token DebugKeyword
%token KeywordKeyword
%token LibraryKeyword
%token Library_onceKeyword
%token EndlibraryKeyword
%token ScopeKeyword
%token EndscopeKeyword
%token StructKeyword
%token EndstructKeyword
%token InterfaceKeyword
%token EndinterfaceKeyword
%token DelegateKeyword
%token StubKeyword
%token ModuleKeyword
%token EndmoduleKeyword
%token ImplementKeyword
%token HookKeyword
%token KeyKeyword
%token ImportKeyword
%token DovjassinitKeyword
%token InjectKeyword
%token EndinjectKeyword
%token NovjassKeyword
%token EndnovjassKeyword
%token VjassKeyword
%token EndvjassKeyword
%token ZincKeyword
%token EndzincKeyword
%token ExternalblockKeyword
%token EndexternalblockKeyword
%token LoaddataKeyword
%token ExternalKeyword
%token TextmacroKeyword
%token TextmacroonceKeyword
%token EndtextmacroKeyword
%token RuntextmacroKeyword
%token RequiresKeyword
%token NeedsKeyword
%token UsesKeyword
%token InitializerKeyword
%token DefaultsKeyword
%token OptionalKeyword
%token SuperKeyword
%token ThistypeKeyword
%token OperatorKeyword
%token ExecuteKeyword
%token EvaluateKeyword
%token GlobalKeyword

%%

IncreaseLine				: NewLine
					{
						parser->currentFile()->addLines(1);
					}
					;

DocComment				: LineDocComment IncreaseLine
					| BlockDocComment /* increase lines please */
					{
					}
					;


Comment					: LineComment IncreaseLine
					| BlockComment /* increase lines please */
					{
						parser->add(new jass::Comment($1, parser->file()->sourceFile(), parser->line());
					}
					;

IntegerOperator				: MultiplicationOperator
					| DivisionOperator
					| PlusOperator
					| MinusOperator
					;

IntegerValue				: IntegerLiteral
					| UnitTypeIntegerLiteral
					| IntegerLiteral IntegerOperator IntegerValue
					| VariableIdentifier IntegerOperator IntegerValue
					| FunctionCall IntegerOperator IntegerValue
					| LeftParenthesis IntegerValue RightParenthesis
					;

RealValue				: RealLiteral
					| IntegerLiteral
					| RealLiteral IntegerOperator RealValue
					| VariableIdentifier IntegerOperator RealValue
					| FunctionCall IntegerOperator RealValue
					| LeftParenthesis RealValue RightParenthesis
					;

StringOperator				: PlusOperator
					;

StringValue				: StringLiteral
					| NullKeyword
					| StringLiteral StringOperator StringValue
					| NullKeyword StringOperator StringValue
					| LeftParenthesis StringValue RightParenthesis
					;

EqualityComparisonOperator		: EqualOperator
					| NotEqualOperator
					;

ComparisonOperator			: EqualityComparisonOperator
					| LessOperator
					| GreaterOperator
					| LessEqualOperator
					| GreaterEqualOperator
					;

BooleanValue				: BooleanLiteral
					| VariableIdentifier
					| FunctionCall
					| IntegerValue ComparisonOperator IntegerValue
					| RealValue ComparisonOperator RealValue
					| StringValue EqualityComparisonOperator StringValue
					| BooleanValue EqualityComparisonOperator BooleanValue
					| LeftParenthesis BooleanValue RightParenthesis
					;

CodeValue				: NullKeyword
					| VariableIdentifier
					| FunctionCall
					| FunctionIdentifier
					| LeftParenthesis CodeValue RightParenthesis
					;

HandleValue				: NullKeyword
					| VariableIdentifier
					| FunctionCall
					| LeftParenthesis HandleValue RightParenthesis
					;

Value					: IntegerValue
					| RealValue
					| StringValue
					| BooleanValue
					| CodeValue
					| HandleValue
					;

ArraySize				: ArrayKeyword LeftSquaredBracked IntegerValue RightSquaredBracked
					;

GlobalDeclaration			: VjassScopePrefix Identifier ArrayKeyword Identifier IncreaseLine
					| VjassScopePrefix Identifier ArrayKeyword Identifier ArraySize IncreaseLine
					| VjassScopePrefix Identifier Identifier IncreaseLine
					| VjassScopePrefix Identifier Identifier SetOperator Value IncreaseLine
					| VjassScopePrefix ConstantKeyword Identifier Identifier SetOperator Value IncreaseLine
					;

GlobalsDeclarations			: GlobalDeclaration GlobalsDeclarations
					| IncreaseLine
					;

GlobalsBlock				: GlobalsKeyword IncreaseLine GlobalsDeclarations EndglobalsKeyword 
					;

LocalDeclaration			: LocalKeyword Identifier Identifier IncreaseLine
					| LocalKeyword Identifier Identifier SetOperator Value IncreaseLine
					;

LocalsDeclarations			: LocalDeclaration LocalsDeclarations
					| IncreaseLine
					;

VariableIdentifier			: Identifier
					| Identifier Dot VariableIdentifier
					;

FunctionIdentifier			: Identifier
					| Identifier Dot FunctionIdentifier
					;

FunctionCallValues			: Value
					| Value Comma FunctionCallValues
					;


FunctionCall				: FunctionIdentifier LeftParenthesis FunctionCallValues RightParenthesis
					;

SetStatement				: SetKeyword VariableIdentifier SetOperator Value IncreaseLine
					;

CallStatement				: CallKeyword FunctionCall IncreaseLine
					;

IfStatement				: IfKeyword BooleanValue ThenKeyword IncreaseLine
					;

ElseifStatement				: ElseifKeyword BooleanValue ThenKeyword IncreaseLine
					;

ExitwhenStatement			: ExitwhenKeyword BooleanValue IncreaseLine
					;

ReturnStatement				: ReturnKeyword Value IncreaseLine
					;

Statement				: SetStatement
					| CallStatement
					| IfStatement
					| ElseifStatement
					| EndifKeyword
					| LoopKeyword
					| ExitwhenStatement
					| EndloopKeyword
					| ReturnStatement
					| DebugKeyword Statement
					;

Parameter				: Identifier Identifier
					;

Parameters				: Parameter
					| Parameter Comma Parameter
					| NothingKeyword
					;

ReturnValue				: Identifier
					| NothingKeyword
					;

FunctionIdentifierLine			: Identifier TakesKeyword Parameters ReturnsKeyword ReturnValue
					;

FunctionInterfaceDeclaration		: VjassScopePrefix FunctionKeyword InterfaceKeyword FunctionIdentifierLine IncreaseLine
					;

FunctionBody				: Statement FunctionBody
					| IncreaseLine
					;

FunctionDeclaration			: VjassScopePrefix FunctionKeyword FunctionIdentifierLine IncreaseLine LocalsDeclarations FunctionBody EndfunctionKeyword IncreaseLine
					;
					

MemberPrefix				: PublicKeyword StaticKeyword ConstantKeyword
					| PrivateKeyword StaticKeyword ConstantKeyword
					| StaticKeyword ConstantKeyword
					| StaticKeyword
					| PublicKeyword
					| PrivateKeyword
					|
					;

MemberDeclaration			: MemberPrefix Identifier Identifier IncreaseLine
					| MemberPrefix Identifier Identifier SetOperator Value IncreaseLine
					| MemberPrefix Identifier ArrayKeyword Identifier ArraySize IncreaseLine
					;

MethodPrefix				: PublicKeyword StaticKeyword ConstantKeyword
					| PrivateKeyword StaticKeyword ConstantKeyword
					| StaticKeyword ConstantKeyword
					| StaticKeyword
					| ConstantKeyword /* non-static methods can also be constant like functions */
					| PublicKeyword
					| PrivateKeyword
					|
					;

MethodDeclaration			: MethodPrefix MethodKeyword FunctionIdentifierLine IncreaseLine LocalsDeclarations FunctionBody EndmethodKeyword
					;

Implementation				: ImplementKeyword OptionalKeyword Identifier
					| ImplementKeyword Identifier
					;

StructArrayExtensionIdentifier		: ArrayKeyword ArraySize
					;

StructDeclarationIdentifier		: Identifier
					| Identifier ExtendsKeyword Identifier
					| Identifier ExtendsKeyword StructArrayExtensionIdentifier
					;

StructBody				: MemberDeclaration IncreaseLine StructBody
					| MethodDeclaration IncreaseLine StructBody
					| Implementation IncreaseLine StructBody
					| IncreaseLine
					;

StructDeclaration			: StructKeyword StructDeclarationIdentifier IncreaseLine StructBody EndstructKeyword IncreaseLine
					;


InterfaceMethodReturnValuePostfix	: ReturnValue SetOperator Identifier
					|
					;

InterfaceMethodDeclaration		: VjassScopePrefix MethodPrefix MethodKeyword FunctionIdentifierLine InterfaceMethodReturnValuePostfix
					;

InterfaceBody				:
					| MemberDeclaration IncreaseLine
					| InterfaceMethodDeclaration IncreaseLine
					| IncreaseLine
					;

InterfaceDeclaration			: InterfaceKeyword Identifier IncreaseLine InterfaceBody EndinterfaceKeyword IncreaseLine
					;

ModuleBody				: MemberDeclaration IncreaseLine ModuleBody
					| MethodDeclaration IncreaseLine ModuleBody
					| IncreaseLine
					;

ModuleDeclaration			: ModuleKeyword Identifier IncreaseLine ModuleBody EndmoduleKeyword IncreaseLine
					;

VjassScopePrefix			: PublicKeyword
					| PrivateKeyword
					|
					;

VjassScopeBody				: FunctionDeclaration VjassScopeBody
					| GlobalsBlock VjassScopeBody
					| StructDeclaration VjassScopeBody
					| InterfaceDeclaration VjassScopeBody
					| ModuleDeclaration VjassScopeBody
					| IncreaseLine
					;

VjassScopeDeclaration			: VjassScopePrefix ScopeKeyword Identifier IncreaseLine VjassScopeBody EndscopeKeyword IncreaseLine
					| VjassScopePrefix ScopeKeyword Identifier InitializerKeyword Identifier IncreaseLine VjassScopeBody EndscopeKeyword IncreaseLine
					;

LibraryPrefix				: LibraryKeyword
					| Library_onceKeyword
					;

LibraryRequirementPrefix		: RequiresKeyword
					| NeedsKeyword
					| UsesKeyword
					;

LibraryRequirement			: Identifier
					| Identifier Comma LibraryRequirement
					| OptionalKeyword Comma LibraryRequirement
					;

VjassLibraryIdentifierLine		: Identifier
					| Identifier InitializerKeyword Identifier
					| Identifier LibraryRequirementPrefix LibraryRequirement
					| Identifier InitializerKeyword Identifier LibraryRequirementPrefix LibraryRequirement
					;

VjassLibraryBody			: FunctionDeclaration VjassScopeBody
					| GlobalsBlock VjassScopeBody
					| StructDeclaration VjassScopeBody
					| InterfaceDeclaration VjassScopeBody
					| ModuleDeclaration VjassScopeBody
					| IncreaseLine
					;

VjassLibraryDeclaration			: LibraryPrefix VjassLibraryIdentifierLine IncreaseLine VjassLibraryBody EndlibraryKeyword IncreaseLine
					;

Preprocessor				: VjassPreprocessor
					;

FileImportLanguage			: ZincKeyword
					| VjassKeyword
					|
					{
					}
					;

FileImport				: VjassPreprocessor ImportKeyword FileImportLanguage StringLiteral IncreaseLine
					;

%%
