#include "../spirit.hpp"
#include <boost/spirit/include/lex_lexertl.hpp>

namespace wc3lib
{
	
namespace jass
{

enum class Token {
	Comment,
	Newline,
	ID,
	String,
	Real,
	Integer
};

namespace lex = boost::spirit::lex;

template <typename Lexer>
struct word_count_tokens : lex::lexer<Lexer>
{
	word_count_tokens() {
		this->self.add
		("//.*", Token::Comment)
		("\n", Token::Newline)
		("\" (. - \"|\\\\.)*\"", Token::String)
		("[0-9]+[1-9]\\.|\\.+[0-9]|[0-9]+[1-9]\\.[0-9]", Token::Real)
		;
	}
};

}

}
