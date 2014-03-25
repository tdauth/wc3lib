#ifndef WC3LIB_JASS_AST_HPP
#define WC3LIB_JASS_AST_HPP

#include <vector>
#include <string>
#include <ostream>
#include <fstream>

#include <boost/spirit/include/qi_symbols.hpp>

namespace wc3lib
{

namespace jass
{

namespace qi = boost::spirit::qi;

struct jass_ast_node {
	virtual std::string type_name() const { return "bla"; }
};

struct jass_hex_integer {
	std::string literal;
};

struct jass_oct_integer {
	std::string literal;
};

struct jass_four_characters {
	char literals[4];
};

struct jass_type : public jass_ast_node {
	std::string identifier;
	jass_type *parent;
	
	jass_type() : parent(0) { }
	jass_type(const std::string &identifier, jass_type *parent) : identifier(identifier), parent(parent) { }
	// if symbol table does not contain the parent type
	jass_type(const std::string &identifier) : identifier(identifier), parent(0) { }
};

typedef boost::variant<std::string> jass_expression_base;

struct jass_expression : public jass_ast_node, public jass_expression_base {
	
};

struct jass_function_declaration : public jass_ast_node {
	std::string identifier;
};

struct jass_var_declaration : public jass_ast_node {
};

struct jass_global : public jass_var_declaration {
	bool is_constant;
	jass_type *type;
	bool is_array;
	std::string identifier;
	jass_ast_node *assignment;
	
	jass_global() : is_constant(false), type(0), is_array(false), assignment(0) {}
	jass_global(jass_type *type, const std::string &identifier) : is_constant(false), type(type), is_array(false), identifier(identifier), assignment(0) { }
};

/*
 * globals
 * 	<vars|constants>
 * endglobals
 * 
 * All global and constant statements have to be placed between "globals" and "endglobals".
 * This node inherits std::vector and can be filled with global and constant declarations.
 */
struct jass_globals : public jass_ast_node, public std::vector<jass_global> {
};

enum class jass_binary_operator {
	Plus,
	Minus,
	Divide,
	Multiply,
	Greater,
	Less,
	Equal,
	NotEqual,
	GreaterEqual,
	LessEqual,
	And,
	Or
};

struct jass_binary_operation : public jass_expression {
	jass_expression *first_expression;
	jass_binary_operator op;
	jass_expression *second_expression;
};

enum class jass_unary_operator {
	Plus,
	Minus,
	Not
};

struct jass_unary_operation : public jass_expression {
	jass_unary_operator op;
	jass_expression *expression;
};

struct jass_function_call : public jass_expression {
	jass_function_declaration *function;
	std::vector<jass_expression*> args;
};

struct jass_array_reference : public jass_expression {
	jass_var_declaration *var;
	jass_expression *index;
};

struct jass_function_reference : public jass_expression {
	jass_function_declaration *function;
};

struct jass_types : qi::symbols<char, jass_type> {
	
	jass_types()
	{
		name("JASS types");
		// add basic JASS types which do not have to be declared
		add
			("integer", jass_type("integer", 0))
			("real", jass_type("real", 0))
			("code", jass_type("code", 0))
			("handle", jass_type("handle", 0))
			("boolean", jass_type("boolean", 0))
			("string", jass_type("string", 0))
		;
	}

	~jass_types() {
	}
};

struct jass_native : public jass_ast_node {
	jass_function_declaration declaration;
};

struct jass_natives : public jass_ast_node, public std::vector<jass_native> {
};

struct jass_declarations : public jass_ast_node {
	jass_types types;
	jass_globals globals;
	jass_natives natives;
};

struct jass_local : public jass_ast_node {
};

struct jass_locals : public jass_ast_node, public std::vector<jass_local> {
};

struct jass_statement : public jass_ast_node {
};

struct jass_statements : public jass_ast_node, public std::vector<jass_statement> {
};

struct jass_function : public jass_ast_node {
	jass_function_declaration declaration;
	jass_locals locals;
	jass_statements statements;
};

struct jass_functions : public jass_ast_node, public std::vector<jass_function> {
};

struct jass_file : public jass_ast_node {
	std::string path;
	jass_declarations declarations;
	jass_functions functions;
};

struct jass_files :  public jass_ast_node, public std::vector<jass_file> {
};

struct jass_ast {
	jass_files files;
};

}

}

/*
 * The specialization is required for proper debug output of Spirit.
 * It prints attributes of rules and therefore attributes of type jass_ast_node which misses a stream output operator.
 * Unfortunately we have to define it for every type because it does not detect inheritance and virtual methods.
 */
// your specialization needs to be in namespace boost::spirit::traits
// https://stackoverflow.com/questions/5286720/how-to-define-streaming-operator-for-boostspiritqi-and-stdlist-container
// https://svn.boost.org/trac/boost/ticket/9803
namespace boost { namespace spirit { namespace traits
{
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_ast_node>
	{
		static void call(Out& out, wc3lib::jass::jass_ast_node const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_local>
	{
		static void call(Out& out, wc3lib::jass::jass_local const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_statement>
	{
		static void call(Out& out, wc3lib::jass::jass_statement const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_native>
	{
		static void call(Out& out, wc3lib::jass::jass_native const& val)
		{
			out << val.type_name();
		}
	};

	// jass_types
	// symbol table
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_types>
	{
		static void call(Out& out, wc3lib::jass::jass_types const& val)
		{
			out << val.name();
		}
	};
}}}

#endif
