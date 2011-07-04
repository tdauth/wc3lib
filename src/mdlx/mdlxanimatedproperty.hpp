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

#ifndef WC3LIB_MDLX_MDLXANIMATEDPROPERTY_HPP
#define WC3LIB_MDLX_MDLXANIMATEDPROPERTY_HPP

#include "mdlxproperty.hpp"

namespace wc3lib
{

namespace mdlx
{

class MdlxAnimatedProperty : public MdlxProperty
{
	public:
		MdlxAnimatedProperty(class MdlxAnimatedProperties *properties, std::size_t valuesCount = 3);
		virtual ~MdlxAnimatedProperty();

		class MdlxAnimatedProperties* properties() const;
		float32 frame() const;
		std::size_t valuesCount() const;
		const std::vector<float32>& values() const;
		const std::vector<float32>& inTan() const;
		const std::vector<float32>& outTan() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		class MdlxAnimatedProperties *m_properties;
		long32 m_frame;
		
		std::size_t m_valuesCount;
		std::vector<float32> m_values;
		//if (LineType > 1) {
		std::vector<float32> m_inTan;
		std::vector<float32> m_outTan;
};

inline class MdlxAnimatedProperties* MdlxAnimatedProperty::properties() const
{
	return this->m_properties;
}

inline float32 MdlxAnimatedProperty::frame() const
{
	return this->m_frame;
}

inline std::size_t MdlxAnimatedProperty::valuesCount() const
{
	return m_valuesCount;
}

inline const std::vector<float32>& MdlxAnimatedProperty::values() const
{
	return m_values;
}

inline const std::vector<float32>& MdlxAnimatedProperty::inTan() const
{
	return m_inTan;
}

inline const std::vector<float32>& MdlxAnimatedProperty::outTan() const
{
	return m_outTan;
}

}

}

#endif
