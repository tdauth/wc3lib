#include <vector>

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include "ast.hpp"
#include "grammar.hpp"
#include "../platform.hpp"
#include "../i18n.hpp"

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

inline void add_to_symbol_table(jass_ast &ast, jass_type &type) {
	// TODO instead of creating a copy, do not use heap based types!
	jass_type *typeCopy = new jass_type(type);
	ast.types.add(type.identifier, typeCopy);
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
		 
		skip %= blank | lit("//") >> *(char_ - (eol | eoi) ) >> (&eol | &eoi); // do not consume eol, since it is required for other rules
		
		 BOOST_SPIRIT_DEBUG_NODES((skip));
	}
	
	qi::rule<Iterator> skip;
};

template <typename Iterator, typename Skipper = comment_skipper<Iterator> >
struct jass_grammar : qi::grammar<Iterator, jass_ast(), qi::locals<std::string>, Skipper>
{
	/**
	 * \param ast An AST must be passed which is filled by the input. It can already contain nodes which might be useful for debugging.
	 */
    jass_grammar(jass_ast &ast)
      : jass_grammar::base_type(jass, "jass"), ast(ast)
    {
	using qi::eps;
        using qi::lit;
	using qi::_val;
        using qi::lexeme;
	using qi::eol;
	using qi::eoi;
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
	
	identifier %= 
		char_("a-zA-Z") >> !(*char_("a-zA-Z_0-9") >> char_("a-zA-Z0-9"))
		;
	
	//lexeme avoids skipping whitespaces which belong to the string
	//string = lexeme['"' >> *alnum >> '"'];
		
	type %=
		lit("type") >> identifier >>
		lit("extends") >> type_symbols // parent type is not optional!
		//>> finish
		;
	
	/*
	expression %=
		lit("true") |
		lit("false") |
	*/
	
	// do not use %= since we have to set the booleans in semantic actions
	global =
		!lit("constant")[at_c<0>(_val) = true] >>
		type_symbols >> 
		!lit("array")[at_c<1>(_val) = true] >>
		identifier
		//>> finish
		//!(char_('=') >> assignment) // TODO add assignment expression, values from symbol table of vars!
	;
	
	globals %=
		lit("globals") >> eol >>
		global % eol >> // % operator is used to generate std::vectors!
		lit("endglobals") // >> finish // end of line or input is always expected after "endglobals"!
	;
	
	jass = //eps[_val = ast] >> // if no input is there generate an empty AST!
		+(globals[at_c<0>(_val) = _1])
		//*(
			//type[ast.types.add]
			//type[phoenix::bind(&client::add_to_symbol_table, _val, _1)] // phoenix::cref(_1) phoenix::ref(_val)
			//type[insert(at_c<0>(_val), _1.identifier, _1)] // add type to AST and to symbol table
			// ,type_symbols.add(_1.identifier, _1)
		//)
		;
	
	/*
	 * put all rule names here:
	 */
	jass.name("jass");
	identifier.name("identifier");
	type.name("type");
	global.name("global");
	globals.name("globals");

        on_error<fail>
        (
            jass
          , std::cerr
                << val("Error! Expecting ")
                << _4                               // what failed?
                << val(" here: \"")
                // TODO Phoenix construct in Boost 1.55
		//<< construct<std::string>(_3, _2)   // iterators to error-pos, end
                << val("\"")
                << std::endl
        );
	
	 BOOST_SPIRIT_DEBUG_NODES((jass)(identifier)(expression)(type)(global)(globals));
    }

	qi::rule<Iterator, jass_ast(), qi::locals<std::string>, Skipper> jass;
	//qi::rule<Iterator, Skipper> finish;
	qi::rule<Iterator, std::string(), Skipper> identifier;
	qi::rule<Iterator, jass_ast_node(), Skipper> expression;
	qi::rule<Iterator, jass_type(), Skipper> type;
	qi::rule<Iterator, jass_global(), Skipper> global;
	qi::rule<Iterator, jass_globals(), Skipper> globals;
	
	// symbol table for types
	// this symbol table can be used in rules! types will be returned automatically when correct identifiers are found!
	jass_ast ast;
	jass_types type_symbols;
};

template <typename Iterator>
bool parse(Iterator first, Iterator last, jass_ast &ast)
{
	jass_grammar<Iterator> grammar(ast);
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

bool Grammar::parse(Grammar::InputStream& istream, jass_ast& ast)
{
	return this->parse(IteratorType(istream), IteratorType(), ast);
}

bool Grammar::parse(IteratorType first, IteratorType last, jass_ast& ast)
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
		result = client::parse(position_begin, position_end, ast);
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

}

}

// We need to tell fusion about our mini_xml struct
// to make it a first-class fusion citizen
//[tutorial_xml1_adapt_structures
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
	wc3lib::jass::jass_ast,
	// (client::jass_types, types)
	(wc3lib::jass::jass_globals, globals)
)
//]
