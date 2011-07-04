#ifndef WC3LIB_LANG_JASSPP_DRIVER_HPP
#define WC3LIB_LANG_JASSPP_DRIVER_HPP

#include <driver.h>

#include "../parser.hpp"

namespace wc3lib
{

namespace lang
{

namespace jasspp
{


class Driver : public example::Driver
{
	public:
		Driver(class CalcContext &calc);
		bool parse_stream(std::istream &in, const std::string &sname = "stream input");
		bool parse_string(const std::string &input, const std::string &sname = "string stream");
		bool parse_file(const std::string &filename);
		/**
		* @todo Should create an instance of class SyntaxError.
		*/
		void error(const class location &l, const std::string &m);
		/**
		* @todo Should create an instance of class SyntaxError.
		*/
		void error(const std::string &m);

};

}

}

}

#endif
