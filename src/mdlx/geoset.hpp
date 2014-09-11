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

#ifndef WC3LIB_MDLX_GEOSET_HPP
#define WC3LIB_MDLX_GEOSET_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "groupmdxblockmember.hpp"
#include "bounds.hpp"
#include "geosets.hpp"

namespace wc3lib
{

namespace mdlx
{

class Geoset : public GroupMdxBlockMember, public Bounds
{
	public:
		enum class Selectable : long32
		{
			None = 0,
			Unselectable = 4
		};

		typedef boost::ptr_vector<class Ganimation> Ganimations;

		Geoset(class Geosets *geosets);
		virtual ~Geoset();

		class Geosets* geosets() const;
		class Vertices* vertices() const;
		class Normals* normals() const;
		class PrimitiveTypes* primitiveTypes() const;
		class PrimitiveSizes* primitiveSizes() const;
		class PrimitiveVertices* primitiveVertices() const;
		class GroupVertices* groupVertices() const;
		class MatrixGroupCounts* matrixGroupCounts() const;
		class Matrices* matrices() const;
		long32 materialId() const;
		long32 selectionGroup() const;
		Selectable selectable() const;
		Ganimations& ganimations();
		const Ganimations& ganimations() const;
		class TexturePatches* texturePatches() const;
		class TextureVertices* textureVertices() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		class Vertices *m_vertices; //VRTX
		class Normals *m_normals; //NRMS
		class PrimitiveTypes *m_primitveTypes; //PTYP
		class PrimitiveSizes *m_primitiveSizes; //PCNT
		class PrimitiveVertices *m_primitiveVertices; //PVTX
		class GroupVertices *m_groupVertices; //GNDX
		class MatrixGroupCounts *m_matrixGroupCounts; //MTGC
		class Matrices *m_matrices; //MATS
		long32 m_materialId;
		long32 m_selectionGroup;
		Selectable m_selectable; //(0:none;4:Unselectable)
		//long32 nanim;
		Ganimations m_ganimations;
		//struct GAnimation *ganimations; //[nganim], naim?!?! Tamino Dauth
		class TexturePatches *m_texturePatches; //UVAS
		class TextureVertices *m_textureVertices; //UVBS
};

inline class Geosets* Geoset::geosets() const
{
	return boost::polymorphic_cast<class Geosets*>(this->parent());
}

inline class Vertices* Geoset::vertices() const
{
	return this->m_vertices;
}

inline class Normals* Geoset::normals() const
{
	return this->m_normals;
}

inline class PrimitiveTypes* Geoset::primitiveTypes() const
{
	return this->m_primitveTypes;
}

inline class PrimitiveSizes* Geoset::primitiveSizes() const
{
	return this->m_primitiveSizes;
}

inline class PrimitiveVertices* Geoset::primitiveVertices() const
{
	return this->m_primitiveVertices;
}

inline class GroupVertices* Geoset::groupVertices() const
{
	return this->m_groupVertices;
}

inline class MatrixGroupCounts* Geoset::matrixGroupCounts() const
{
	return this->m_matrixGroupCounts;
}

inline class Matrices* Geoset::matrices() const
{
	return this->m_matrices;
}

inline long32 Geoset::materialId() const
{
	return this->m_materialId;
}

inline long32 Geoset::selectionGroup() const
{
	return this->m_selectionGroup;
}

inline Geoset::Selectable Geoset::selectable() const
{
	return this->m_selectable;
}

inline Geoset::Ganimations& Geoset::ganimations()
{
	return this->m_ganimations;
}

inline const Geoset::Ganimations& Geoset::ganimations() const
{
	return this->m_ganimations;
}

inline class TexturePatches* Geoset::texturePatches() const
{
	return this->m_texturePatches;
}

inline class TextureVertices* Geoset::textureVertices() const
{
	return this->m_textureVertices;
}

}

}

#endif
