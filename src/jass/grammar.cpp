#include <vector>

#include "ast.hpp"
#include "grammar.hpp"

/*
 * Define this static attribute before including Spirit stuff since we define it as our custom debugging output stream
 * for Spirit traces.
 */
namespace wc3lib
{

namespace jass
{

std::ofstream Grammar::traceLog;

}

}

#ifdef DEBUG
#define BOOST_SPIRIT_DEBUG // enable debugging for Qi
// we use the class attribute as custom output stream for Spirit's debugging traces
// the public attribute can be opened in each unit test customly where the user wants to have the output traces stored
#define BOOST_SPIRIT_DEBUG_OUT wc3lib::jass::Grammar::traceLog
#endif

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/lexical_cast.hpp>

#include "../platform.hpp"
#include "../i18n.hpp"

// http://www.boost.org/doc/libs/1_55_0/libs/spirit/doc/html/spirit/advanced/customize/transform.html
// http://www.boost.org/doc/libs/1_55_0/libs/spirit/doc/html/spirit/qi/reference/auxiliary/attr_cast.html
// http://www.boost.org/doc/libs/1_55_0/libs/spirit/doc/html/spirit/advanced/customize/transform.html
// https://stackoverflow.com/questions/13515623/boost-spirit-how-to-convert-basic-types
namespace boost { namespace spirit { namespace traits
{

/*
 * For conversion from real literals to real numbers.
 */
template <>
struct transform_attribute<std::string, double, qi::domain>
{
	typedef double type;
	
	static double pre(const std::string & d) {
		// TODO does it detect .343 properly?
		return boost::lexical_cast<double>(d);
	}
	static void post(const std::string & val, double const& attr) {}
	static void fail(std::string&) {}
};

// decimal int
/*
 * Left rule is the resulting integer.
 * Right expected value is a string (lexeme).
 */
/*
template <>
struct transform_attribute<int, std::string, qi::domain>
{
	// input type
	typedef std::string& type;
	
	static std::string pre(int & d) {
		return "";
	}
	static void post(int & val, const type attr) {
		val = boost::lexical_cast<int>(attr);
	}
	static void fail(int &) {}
};
*/

// octal integer has to be calculated correctly to get a decimal integer
template <>
struct transform_attribute<wc3lib::jass::jass_oct_integer, int, qi::domain>
{
	typedef int type;
	
	static int pre(const wc3lib::jass::jass_oct_integer & d) {
		int result = 0;
		
		for (int i = d.literal.size() - 1; i >= 0 ; --i) {
			result += boost::lexical_cast<int>(d.literal[i]) * std::pow(8, i);
		}
		
		return result;
	}
	static void post(const wc3lib::jass::jass_oct_integer & val, bool const& attr) {}
	static void fail(wc3lib::jass::jass_oct_integer&) {}
};

// same for hexadecimal integer
template <>
struct transform_attribute<wc3lib::jass::jass_hex_integer, int, qi::domain>
{
	typedef int type;
	
	static int pre(const wc3lib::jass::jass_hex_integer & d) {
		int result = 0;
		
		for (int i = d.literal.size() - 1; i >= 0; --i) {
			result += boost::lexical_cast<int>(d.literal[i]) * std::pow(16, i);
		}
		
		return result;
	}
	static void post(const wc3lib::jass::jass_hex_integer & val, bool const& attr) {}
	static void fail(wc3lib::jass::jass_hex_integer&) {}
};
	
// in this case we just expose the embedded 'int' as the attribute instance
// to use, allowing to leave the function 'post()' empty
template <>
struct transform_attribute<std::string, bool, qi::domain>
{
	typedef bool type;
	
	static bool pre(const std::string & d) { 
		return d == "true" ? true : false;
	}
	static void post(const std::string & val, bool const& attr) {}
	static void fail(std::string&) {}
};

}}}

namespace wc3lib
{

namespace jass
{

namespace client {

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename T>
struct symbol_deleter
{
    void operator() (T *value)
    {
        delete value;
    }
};

inline void add_to_symbol_table(jass_declarations &declarations, jass_type &type) {
	declarations.types.add(type.identifier, type);
}

inline jass_type* get_type_symbol(jass_types &types, const std::string &value) {
	std::cout << "Searching for type " << value << std::endl;
	
	return types.find(value.c_str());
}

/*
 * Doesn't consume eols since value pairs are separated linewise which therefore can be specified easier in the rules
 */
template<typename Iterator>
struct comment_skipper : public qi::grammar<Iterator> {
	
	comment_skipper() : comment_skipper::base_type(skip, "comments and blanks")
	{
		using qi::lit;
		using ascii::char_;
		using ascii::blank;
		using qi::eol;
		using qi::eoi;
		
		comment %=
			lit("//") >> *(char_ - eol)
			;
		
		emptylines %=
			(*blank >> -comment) % (+eol)
			;
		
		skip %= 
			blank |
			comment
			; 
		
		emptylines.name("emptylines");
		comment.name("comment");
		skip.name("skip");
		
		BOOST_SPIRIT_DEBUG_NODES((emptylines)(comment)(skip));
	}
	
	qi::rule<Iterator> skip;
	qi::rule<Iterator> emptylines;
	qi::rule<Iterator> comment;
};

template <typename Iterator, typename Skipper = comment_skipper<Iterator> >
struct jass_grammar : qi::grammar<Iterator, jass_ast(), qi::locals<std::string>, Skipper>
{
	/**
	 * \param ast An AST must be passed which is filled by the input. It can already contain nodes which might be useful for debugging.
	 */
    jass_grammar(jass_ast &ast, jass_file &current_file)
      : jass_grammar::base_type(jass, "jass"), ast(ast), current_file(current_file)
    {
	using qi::eps;
	using qi::int_parser;
	using qi::double_;
        using qi::lit;
	using qi::oct;
	using qi::hex;
	using qi::true_;
	using qi::false_;
	using qi::_val;
        using qi::lexeme;
	using qi::eol;
	using qi::eoi;
	using qi::attr_cast;
	using qi::as_string;
        using qi::on_error;
        using qi::fail;
        using ascii::char_;
        using ascii::string;
        using namespace qi::labels;

        //using phoenix::construct;
        using phoenix::val;
	using phoenix::at_c;
	using phoenix::push_back;
	using phoenix::ref;

	/*
        text %= lexeme[+(char_ - '<')];
        node %= xml | text;

        start_tag %=
                '<'
            >>  !lit('/')
            >   lexeme[+(char_ - '>')]
            >   '>'
        ;

        end_tag =
                "</"
            >   string(_r1)
            >   '>'
        ;

        xml %=
                start_tag[_a = _1]
            >   *node
            >   end_tag(_a)
        ;
*/
	// _val is the attribute which is being synthesized, so we set the identifier to the parsed string
	// lexeme includes space skipping: lexeme[]
	// %= writes automatically the expression into the attribute if possible. It's the same as writing [_val = _1]!
	// r %= p and r = p are equivalent if there are no semantic actions associated with p.

	// use the following statement for creating pointer values:
	// [qi::_val = phoenix::new_<Num>( ]
	
	/*finish %=
		(eol | eoi)
		;
	*/
	
	/*
	 * For literals we do not use parsers provided by Boost Spirit to keep the exact grammar definition
	 * and to be more flexible in type representation of integers and reals etc.
	 */
	
	integer_literal %=
		qi::int_parser<int32>() |
		-(lit('$') | lit('0') >> (lit('x') | lit('X'))) >> hex | // TODO custom type int32?
		oct // TODO custom type int32?
		/*
		as_string[
		lexeme[
			-(char_('+') | char_('-')) >>
			char_("1-9") >> *char_("0-9")
		]
		]
		*/
		;
	
	real_literal %=
		double_
		;
		/*
		attr_cast(
		lexeme[
			-(char_('+') | char_('-')) >> +char_("0-9") >> -(lit('.') >> *char_("0-9")) | 
			char_('.') >> +char_("0-9")
		]
		)
		;
		*/
		
	string_literal %=
		lexeme[
			lit("\"") >>
			*(~char_("\"") | char_("\\\"")) >>
			lit("\"")
		]
		;
	
	boolean_literal %=
		true_ |
		false_
		/*
		attr_cast( // cast string literal into boolean
		lexeme[
			lit("true") |
			lit("false")
		]
		)
		*/
		;
	
	identifier = 
		lexeme[char_("a-zA-Z") >> -(*char_("a-zA-Z_0-9") >> char_("a-zA-Z0-9"))]
		;
	
	type_identifier =
		identifier[_val = phoenix::bind(&get_type_symbol, ref(type_symbols), _1)]
		;
	
	//lexeme avoids skipping whitespaces which belong to the string
	//string = lexeme['"' >> *alnum >> '"'];
		
	type %=
		lit("type") >> identifier >>
		lit("extends") >> type_identifier // parent type is not optional!
		;
	
	/*
	expression %=
		lit("true") |
		lit("false") |
	*/
	
	// do not use %= since we have to set the booleans in semantic actions
	global =
		-lit("constant")[at_c<0>(_val) = true] >>
		type_identifier[at_c<2>(_val) = _1] >>
		-lit("array")[at_c<1>(_val) = true] >>
		identifier[at_c<3>(_val) = _1]
		//!(char_('=') >> assignment) // TODO add assignment expression, values from symbol table of vars!
	;
	
	globals %=
		lit("globals") >> +eol >>
		global % +eol >> // % operator is used to generate std::vectors!
		+eol >>
		lit("endglobals") // >> finish // end of line or input is always expected after "endglobals"!
	;
	
	declarations =
		eps[_val = jass_declarations()] >>
		(
			globals[at_c<1>(_val) = _1] |
			//type[push_back(at_c<0>(_val), _1)]
			type[phoenix::bind(&client::add_to_symbol_table, _val, _1)]
		)
		% eol
	;
	
	function_declaration %=
		identifier >>
		lit("takes") >>
		// TODO args
		lit("returns")
		// TODO return type
		;
	
	//locals %=
	//	;
		
	//statements %=
	//	;
	
	function %=
		lit("function") >>
		function_declaration
		 >> eol >>
		locals >> eol >>
		statements >> eol >>
		lit("endfunction")
		;
		
	functions %=
		function % eol
		;
		
	
	file %= //eps[_val = ast] >> // if no input is there generate an empty AST!
		*eol >>
		-declarations >>
		*eol >>
		-functions >>
		//*(
			//type[ast.types.add]
			//type[phoenix::bind(&client::add_to_symbol_table, _val, _1)] // phoenix::cref(_1) phoenix::ref(_val)
			//type[insert(at_c<0>(_val), _1.identifier, _1)] // add type to AST and to symbol table
			// ,type_symbols.add(_1.identifier, _1)
		//)
		
		*eol
		;
	
	/*
	 * Parses the current file
	 */
	jass = 
		eps[_val = ast] >>
		file
		;
	
	
	/*
	 * put all rule names here:
	 */
	jass.name("jass");
	file.name("file");
	identifier.name("identifier");
	type_identifier.name("type_identifier");
	type.name("type");
	global.name("global");
	globals.name("globals");
	declarations.name("declarations");
	function_declaration.name("function_declaration");
	locals.name("locals");
	statements.name("statements");
	function.name("function");
	functions.name("functions");

	on_error<fail>
	(
		file,
		std::cerr
			<< val("Error! Expecting ")
			<< _4                               // what failed?
			<< val(" here: \"")
			// TODO Phoenix construct in Boost 1.55
			//<< construct<std::string>(_3, _2)   // iterators to error-pos, end
			<< val("\"")
			<< std::endl
	);
	
	BOOST_SPIRIT_DEBUG_NODES((jass)(file)(identifier)(type_identifier)(expression)(type)(global)(globals)(declarations)(function_declaration)(locals)(statements)(function)(functions));
    }

	qi::rule<Iterator, jass_ast(), qi::locals<std::string>, Skipper> jass;
	qi::rule<Iterator, jass_file(), Skipper> file;
	//qi::rule<Iterator, Skipper> finish;
	qi::rule<Iterator, int32(), Skipper> integer_literal;
	qi::rule<Iterator, float32(), Skipper> real_literal;
	qi::rule<Iterator, string(), Skipper> string_literal;
	qi::rule<Iterator, bool(), Skipper> boolean_literal;
	qi::rule<Iterator, std::string(), Skipper> identifier;
	qi::rule<Iterator, jass_type*(), Skipper> type_identifier;
	qi::rule<Iterator, jass_ast_node(), Skipper> expression;
	qi::rule<Iterator, jass_type(), Skipper> type;
	qi::rule<Iterator, jass_global(), Skipper> global;
	qi::rule<Iterator, jass_globals(), Skipper> globals;
	qi::rule<Iterator, jass_declarations(), Skipper> declarations;
	qi::rule<Iterator, jass_function_declaration(), Skipper> function_declaration;
	qi::rule<Iterator, jass_locals(), Skipper> locals;
	qi::rule<Iterator, jass_statements(), Skipper> statements;
	qi::rule<Iterator, jass_function(), Skipper> function;
	qi::rule<Iterator, jass_functions(), Skipper> functions;
	
	// symbol table for types
	// this symbol table can be used in rules! types will be returned automatically when correct identifiers are found!
	jass_ast ast;
	jass_file current_file; // currently parsed file
	jass_types type_symbols;
};

template <typename Iterator>
bool parse(Iterator first, Iterator last, jass_ast &ast, jass_file &current_file)
{
	jass_grammar<Iterator> grammar(ast, current_file);
	comment_skipper<Iterator> commentSkipper;
	
	bool r = boost::spirit::qi::phrase_parse(
	first,
	last,
	grammar,
	commentSkipper,
	ast // store result into the passed ast
	);

	if (first != last) // fail if we did not get a full match
	{
		return false;
	}

	return r;
}

}

bool Grammar::parse(Grammar::InputStream& istream, jass_ast& ast, jass_file &file)
{
	return this->parse(IteratorType(istream), IteratorType(), ast, file);
}

bool Grammar::parse(IteratorType first, IteratorType last, jass_ast& ast, jass_file &file)
{
	ForwardIteratorType forwardFirst = boost::spirit::make_default_multi_pass(first);
	ForwardIteratorType forwardLast = boost::spirit::make_default_multi_pass(last); // TODO has to be created? Do we need this iterator to be passed?
	
	// used for backtracking and more detailed error output
	namespace classic = boost::spirit::classic;
	typedef classic::position_iterator2<ForwardIteratorType> PositionIteratorType;
	PositionIteratorType position_begin(forwardFirst, forwardLast);
	PositionIteratorType position_end;
	bool result = false;
	
	try {
		result = client::parse(position_begin, position_end, ast, file);
	} catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> &e)
	{
		const classic::file_position_base<std::string>& pos = e.first.get_position();
		std::stringstream msg;
		msg <<
		std::setw(pos.column) << " " << _("^- here");
		
		throw Exception(
			boost::format(_("Parse error at file %1%:%2%:%3%: %4%\n%5%")) % pos.file % pos.line % pos.column % e.first.get_currentline() % msg.str()
			);
	}
	
	return result;
}

bool Grammar::parse(Grammar::InputStream& istream, jass_ast& ast, const std::string& fileName)
{
	jass_file file;
	file.path = fileName;
	ast.files.push_back(file);
	
	return this->parse(IteratorType(istream), IteratorType(), ast, file);
}

}

}

/*
 * All parsed structures have to be defined manually for Fusion.
 * Only add attributes which should be parsed!
 */

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_type,
	(std::string, identifier)
	(wc3lib::jass::jass_type*, parent)
)

/*
 * define in parsing order so define type first for example!
 */
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_global,
	(bool, is_constant) //don't expect is_constant for the struct, it is set manually in the semantic action because we need a boolean value
	(bool, is_array) //the same as for is_constant
	(wc3lib::jass::jass_type*,type)
	(std::string, identifier)
	//(jass_ast_node*, assignment) TODO add assignment?
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function_declaration,
	(std::string, identifier)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function,
	(wc3lib::jass::jass_function_declaration, declaration)
	(wc3lib::jass::jass_locals, locals)
	(wc3lib::jass::jass_statements, statements)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_declarations,
	(wc3lib::jass::jass_types, types)
	(wc3lib::jass::jass_globals, globals)
	(wc3lib::jass::jass_natives, natives)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_file,
	(wc3lib::jass::jass_declarations, declarations)
	(wc3lib::jass::jass_functions, functions)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_ast,
	// (client::jass_types, types)
	// TODO do we have to fill these?
	//(wc3lib::jass::jass_files, files)
)
//]
