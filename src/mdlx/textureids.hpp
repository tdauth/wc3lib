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

#ifndef WC3LIB_MDLX_TEXTUREIDS_HPP
#define WC3LIB_MDLX_TEXTUREIDS_HPP

#include "mdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "KMTF".
 * MDL tag "TextureID".
 * State of TextureId is long not float, therefore this class does not inherit \ref MdlxAlphas.
 */
class TextureIds : public MdxBlock
{
	public:
		typedef std::list<class TextureId*> Ids;
		
		TextureIds(class Layer *layer);
		virtual ~TextureIds();

		class Layer* layer() const;
		BOOST_SCOPED_ENUM(LineType) lineType() const;
		long32 globalSequenceId() const;
		const Ids& textureIds() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		class Layer *m_layer;
		BOOST_SCOPED_ENUM(LineType) m_lineType;
		long32 m_globalSequenceId; // 0xFFFFFFFF if none
		Ids m_textureIds;
};

inline class Layer* TextureIds::layer() const
{
	return this->m_layer;
}

inline BOOST_SCOPED_ENUM(LineType) TextureIds::lineType() const
{
	return this->m_lineType;
}

inline long32 TextureIds::globalSequenceId() const
{
	return this->m_globalSequenceId;
}

inline const TextureIds::Ids& TextureIds::textureIds() const
{
	return this->m_textureIds;
}

}

}

#endif
