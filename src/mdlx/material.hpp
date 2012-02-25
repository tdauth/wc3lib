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

#ifndef WC3LIB_MDLX_MATERIAL_HPP
#define WC3LIB_MDLX_MATERIAL_HPP

#include "groupmdxblockmember.hpp"
#include "materials.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Material".
 */
class Material : public GroupMdxBlockMember
{
	public:
		BOOST_SCOPED_ENUM_START(RenderMode) /// \todo C++11 : long32
		{
			ConstantColor = 1,
			Unknown0 = 2,
			Unknown1 = 4,
			SortPrimitivesNearZ = 8,
			SortPrimitivesFarZ = 16,
			FullResolution = 32
		};
		BOOST_SCOPED_ENUM_END

		Material(class Materials *materials);
		virtual ~Material();

		class Materials* materials() const;
		/**
		 * Taken from Art Tool manual:
		 * Alpha-queued geosets can be made to draw in a specific order with relation to each other in the same model. The lower the value is, the sooner it is rendererd. Values between -20 and 20 are regularly used.
		 */
		long32 priorityPlane() const;
		BOOST_SCOPED_ENUM(RenderMode) renderMode() const;
		class Layers* layers() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		//long nbytesi;
		long32 m_priorityPlane;
		BOOST_SCOPED_ENUM(RenderMode) m_renderMode;
		class Layers *m_layers;
};

inline class Materials* Material::materials() const
{
	return boost::polymorphic_cast<class Materials*>(this->parent());
}

inline long32 Material::priorityPlane() const
{
	return this->m_priorityPlane;
}

inline BOOST_SCOPED_ENUM(Material::RenderMode) Material::renderMode() const
{
	return this->m_renderMode;
}

inline class Layers* Material::layers() const
{
	return this->m_layers;
}

}

}

#endif
