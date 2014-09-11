/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_TEXTUREPATCHES_HPP
#define WC3LIB_MDLX_TEXTUREPATCHES_HPP

#include "mdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "UVAS"
 * No MDL tag!
 */
class TexturePatches : public MdxBlock
{
	public:
		TexturePatches(class Geoset *geoset);
		virtual ~TexturePatches();

		class Geoset* geoset() const;
		long32 number() const;

		virtual std::streamsize readMdl(InputStream &istream);
		virtual std::streamsize writeMdl(OutputStream &ostream) const;
		virtual std::streamsize readMdx(InputStream &istream);
		virtual std::streamsize writeMdx(OutputStream &ostream) const;

	protected:
		class Geoset *m_geoset;
		long32 m_number;
};

inline class Geoset* TexturePatches::geoset() const
{
	return this->m_geoset;
}

inline long32 TexturePatches::number() const
{
	return this->m_number;
}

}

}

#endif
