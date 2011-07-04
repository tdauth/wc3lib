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

#ifndef WC3LIB_MDLX_MDLXANIMATEDPROPERTIES_HPP
#define WC3LIB_MDLX_MDLXANIMATEDPROPERTIES_HPP

#include "mdlxproperty.hpp"
#include "mdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

class MdlxAnimatedProperties : public MdxBlock
{
	public:
		typedef std::list<class MdlxAnimatedProperty*> Properties;
		MdlxAnimatedProperties(class Mdlx *mdlx, const byte mdxIdentifier[MdxBlock::mdxIdentifierSize], const string &mdlIdentifier, bool optional = true);
		virtual ~MdlxAnimatedProperties();

		class Mdlx* mdlx() const;
		BOOST_SCOPED_ENUM(LineType) lineType() const;
		long32 globalSequenceId() const;
		bool hasGlobalSequence() const;
		const Properties& properties() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		virtual class MdlxAnimatedProperty* createAnimatedProperty() = 0;
		
		class Mdlx *m_mdlx;
		BOOST_SCOPED_ENUM(LineType) m_lineType; //(0:don't interp;1:linear;2:hermite;3:bezier)
		long32 m_globalSequenceId; // 0xFFFFFFFF if none
		Properties m_properties;
};

inline class Mdlx* MdlxAnimatedProperties::mdlx() const
{
	return this->m_mdlx;
}

inline BOOST_SCOPED_ENUM(LineType) MdlxAnimatedProperties::lineType() const
{
	return this->m_lineType;
}

inline long32 MdlxAnimatedProperties::globalSequenceId() const
{
	return this->m_globalSequenceId;
}

inline bool MdlxAnimatedProperties::hasGlobalSequence() const
{
	return this->m_globalSequenceId != 0xFFFFFFFF;
}

inline const MdlxAnimatedProperties::Properties& MdlxAnimatedProperties::properties() const
{
	return this->m_properties;
}

}

}

#endif

