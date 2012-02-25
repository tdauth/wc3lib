/***************************************************************************
 *   Copyright (C) 2008 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef WC3LIB_LANG_COMPILER_HPP
#define WC3LIB_LANG_COMPILER_HPP

#include <ostream>

#include <boost/filesystem.hpp>

#include "../exception.hpp"

namespace wc3lib
{

namespace lang
{

class Compiler
{
	public:
		/**
		* Available optimization options which can be combined.
		* Allows user to create more efficient and unreadable code.
		*/
		enum Optimization
		{
			None = 0x00,
			RemoveComments = 0x0002, /// All comments will be ommitted.
			ReplaceIdentifiers = 0x0004, /// All identifiers will be replaced by short identifiers like "a".
			ReplaceConstants = 0x0008, /// All constants identifiers will be replaced by their content and not be declared in map script.
			InlineFunctions = 0x0012
		};

		enum DocumentationFormats
		{
			Html = 0x02,
			Latex = 0x04
		};

		Compiler();
		~Compiler();

		/**
		* Compiles parsed tokens of parser @param parser into output stream @param ostream.
		* When compiling the code a map script is always being generated.
		* The following list shows which scripting language code can be compiled into which other scripting language code:
		* <ul>
		* <li>JASS++ -> JASS</li>
		* <li>vJass -> JASS</li>
		* <li>Zinc -> vJass</li>
		* <li>cJass -> JASS</li>
		* <ul>
		* Note that JASS code will be formed as a map script, too.
		* Those scripting languages can all be combined.
		* @param parser Parser which holds all parsed tokens. It should not be changed. When changing idenfitiers for example there should be made some copies.
		* @param optimization Used optimization options.
		*/
		void compile(std::ostream &ostream, const class Parser &parser, enum Optimization optimization = None) throw (class Exception);
		/**
		* Generates an API documentation in directory @param directory using parsed tokens of parser @param parser and generating files in formats @param documentationFormats.
		*/
		void generateDocumentation(boost::filesystem::path &directory, const class Parser &parser, enum DocumentationFormats documentationFormats = Html) throw (class Exception);

	protected:
		Compiler(const Compiler &);
		Compiler& operator=(const Compiler &);

		//void generateArrayAccessFunctions();
		//void generateFunctionPrototypes();

		/**
		* Writes "the global hashtable". Jass++ only.
		*/
		/*
		void writeHashtableGlobal(std::ostream &ostream);
		void writeGlobals(std::ostream &ostream);
		void writeFunctions(std::ostream &ostream);

		/**
		* Jass++
		* Writes all globals (including member, function prototype and array globals etc.)
		*/
		/*
		void writePackageGlobals(std::ostream &ostream);
		void writePackageFunctions(std::ostream);


		std::list<class FunctionPrototype*> m_prototypes;
		*/
};

}

}

#endif
