#ifndef WC3LIB_LANG_POSITION_HPP
#define WC3LIB_LANG_POSITION_HPP

namespace wc3lib
{

namespace lang
{

/**
* Provides access to the position in a source file.
*/
class Position
{
	public:
		Position(class SourceFile *sourceFile, std::size_t line = 0, std::size_t column = 0);
		std::size_t line() const;
		std::size_t column() const;

	private:
		class SourceFile *m_sourceFile;
		std::size_t m_line;
		std::size_t m_column;
};

inline std::size_t Position::line() const
{
	return this->m_line;
}

inline std::size_t Position::column() const
{
	return this->m_column;
}

}

}

#endif
