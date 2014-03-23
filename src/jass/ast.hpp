#ifndef WC3LIB_JASS_AST_HPP
#define WC3LIB_JASS_AST_HPP

#include <vector>
#include <string>

#include <boost/spirit/include/qi_symbols.hpp>

namespace wc3lib
{

namespace jass
{

namespace qi = boost::spirit::qi;

struct jass_ast_node {
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

struct jass_types : qi::symbols<char, jass_type*>
{
    jass_types()
    {
	name("JASS types");
	// add basic JASS types which do not have to be declared
	add
        ("integer", new jass_type("integer", 0))
	("real", new jass_type("real", 0))
	("code", new jass_type("code", 0))
	("handle", new jass_type("handle", 0))
	("boolean", new jass_type("boolean", 0))
	("string", new jass_type("string", 0))
        ;
    }
    
    ~jass_types() {
	    // TODO delete all
	   // symbol_deleter<jass_type> deleter;
	   // this->for_each(deleter);
    }

};

struct jass_ast {
	jass_types types;
	jass_globals globals;
};

}

}

#endif
