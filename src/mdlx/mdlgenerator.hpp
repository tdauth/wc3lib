/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_MDLGENERATOR_HPP
#define WC3LIB_MDLX_MDLGENERATOR_HPP

/**
 * \file
 * \ingroup mdlsupport
 */

#include "../spirit.hpp"
#include <boost/spirit/include/karma.hpp>

#include <iterator>

#include "../platform.hpp"
#include "../exception.hpp"
#include "mdlgrammarclient.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \brief Generator for MDL files.
 *
 * This class supports generating an MDL file from an \ref Mdlx object.
 *
 * \ingroup mdlsupport
 */
class MdlGenerator
{
	public:
		typedef std::basic_ostream<byte> OutputStream;
		typedef std::ostreambuf_iterator<byte> IteratorType;

		MdlGenerator();

		/**
		 * Generates an MDL file into \p ostream from model \p source.
		 * \return Returns true if generation succeeded.
		 */
		bool generate(OutputStream &istream, const Mdlx &source);
		bool generate(IteratorType sink, const Mdlx &source);

	private:
		/*
		 * Internal grammars for MDL grammar.
		 * We do not want to create a new grammar instance each time we parse something.
		 * Therefore we use these attributes for all parsing operations.
		 * Make it static that it only has to be allocated once which improves the performance when using multiple instances of class MdlGenerator.
		 */
		static const client::MdlGenerator<IteratorType> grammar;
};

}

}

#endif
