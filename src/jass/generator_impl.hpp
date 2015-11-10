/***************************************************************************
 *   Copyright (C) 2015 by Tamino Dauth                                    *
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

#ifndef WC3LIB_JASS_GENERATORIMPL_HPP
#define WC3LIB_JASS_GENERATORIMPL_HPP

#include <ostream>

#include "../spirit.hpp"

#include <boost/spirit/include/karma.hpp>

#include "client.hpp"

namespace wc3lib
{

namespace jass
{

/**
 * \brief A generator for the scripting language JASS.
 *
 * This class allows to generate JASS code output from a given AST.
 * Use \ref generate() to generate such JASS code output.
 */
class GeneratorImpl
{
	public:
		typedef std::basic_ostream<byte> OutputStream;
		typedef std::ostreambuf_iterator<char> IteratorType;
		typedef client::jass_generator<IteratorType> JassGenerator;

		GeneratorImpl();

		/**
		 * Generates a JASS file into \p ostream from AST \p ast.
		 * \return Returns true if generation succeeded.
		 */
		bool generate(OutputStream &ostream, const jass_ast &ast);
		bool generate(IteratorType sink, const jass_ast &ast);

	private:
		/*
		 * Internal generator for JASS generator.
		 * We do not want to create a new generator instance each time we generate something.
		 * Therefore we use these attributes for all generating operations.
		 * Make it static that it only has to be allocated once which improves the performance when using multiple instances of class \ref Generator.
		 */
		static const JassGenerator generator;
};

}

}

#endif