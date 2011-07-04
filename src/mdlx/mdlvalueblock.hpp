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

#error Deprecated!
#ifndef WC3LIB_MDLX_MDLVALUEBLOCK_HPP
#define WC3LIB_MDLX_MDLVALUEBLOCK_HPP

#include "mdlblock.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL value blocks are usually used for animated data:
 * \code
 Tag <long_count> {
	<DontInterp|Linear|Hermite|Bezier>,
		GlobalSeqId <long>,
		<long_frame>: <VALUES>,
			InTan <VALUES>,
			OutTan <VALUES>,
		...
	}
 * \endcode
 */
template<typename T>
class MdlValueBlock : public MdlBlock
{
	public:
		typedef T ValueType;
		
		/// \param value Initial null value.
		MdlValueBlock(const string &mdlIdentifier, T value, bool optional = true);
		virtual ~MdlValueBlock();
		
		ValueType value() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		
	protected:
		ValueType m_value;
};

template<typename T>
inline T MdlValueBlock<T>::value() const
{
	return m_value;
}

}

}

#include "mdlvalueblock.cpp"

#endif
