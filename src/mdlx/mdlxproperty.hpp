/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_MDLXPROPERTY_HPP
#define WC3LIB_MDLX_MDLXPROPERTY_HPP

#include "../format.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * Abstract class for all MDLX format properties.
 * All properties require MDL and MDX read and write functions.
 * Default read and write functions and stream operators (\ref MdlxProperty::read and \ref MdlxProperty::write) use MDX format (not MDL!).
 */
class MdlxProperty : public Format
{
	public:
		virtual std::streamsize readMdl(istream &istream) = 0;
		virtual std::streamsize writeMdl(ostream &ostream) const = 0;
		
		virtual std::streamsize readMdx(istream &istream) = 0;
		virtual std::streamsize writeMdx(ostream &ostream) const = 0;
		
		/**
		 * \brief Default read and write format is MDX.
		 */
		virtual inline std::streamsize read(istream &istream) { return readMdx(istream); };
		/**
		 * \copybrief MdlxProperty::read
		 */
		virtual inline std::streamsize write(ostream &ostream) const { return writeMdx(ostream); };
};

}

}

#endif
