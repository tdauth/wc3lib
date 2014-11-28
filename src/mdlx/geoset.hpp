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

#include <vector>

#include "bounds.hpp"
#include "faces.hpp"
#include "matrix.hpp"


namespace wc3lib
{

namespace mdlx
{

class Geoset : public Bounds
{
	public:
		enum class Selectable : long32
		{
			None = 0,
			Unselectable = 4
		};

		typedef std::vector<VertexData> Vertices;
		typedef std::vector<VertexData> Normals;
		typedef std::vector<wc3lib::mdlx::Faces> Faces;
		typedef std::vector<uint8> GroupVertices;
		typedef std::vector<Matrix> Matrices;
		typedef std::vector<Bounds> Ganimations;
		typedef std::vector<Vertex2d<float32> > TexturePatches;
		typedef std::vector<Vertex2d<float32> > TextureVertices;

		Geoset();

		void setVertices(const Vertices &vertices);
		const Vertices& vertices() const;
		void setNormals(const Normals &normals);
		const Normals& normals() const;
		void setFaces(const Faces &faces);
		const Faces& faces() const;
		void setGroupVertices(const GroupVertices &groupVertices);
		const GroupVertices& groupVertices() const;
		void setMatrices(const Matrices &matrices);
		const Matrices& matrices() const;
		void setMaterialId(long32 materialId);
		long32 materialId() const;
		void setSelectionGroup(long32 selectionGroup);
		long32 selectionGroup() const;
		void setSelectable(Selectable selectable);
		Selectable selectable() const;
		void setGanimations(const Ganimations &ganimations);
		const Ganimations& ganimations() const;
		void setTexturePatches(const TexturePatches &texturePatches);
		const TexturePatches& texturePatches() const;
		void setTextureVertices(const TextureVertices &textureVertices);
		const TextureVertices& textureVertices() const;

	protected:
		Vertices m_vertices; //VRTX
		Normals m_normals; //NRMS
		Faces m_faces; // primitives
		GroupVertices m_groupVertices; //GNDX
		Matrices m_matrices; //MATS
		long32 m_materialId;
		long32 m_selectionGroup;
		Selectable m_selectable; //(0:none;4:Unselectable)
		//long32 nanim;
		Ganimations m_ganimations;
		//struct GAnimation ganimations; //[nganim], naim?!?! Tamino Dauth
		TexturePatches m_texturePatches; //UVAS
		TextureVertices m_textureVertices; //UVBS
};

inline void Geoset::setVertices(const Geoset::Vertices &vertices)
{
	this->m_vertices = vertices;
}

inline const Geoset::Vertices& Geoset::vertices() const
{
	return this->m_vertices;
}

inline void Geoset::setNormals(const Geoset::Normals &normals)
{
	this->m_normals = normals;
}

inline const Geoset::Normals& Geoset::normals() const
{
	return this->m_normals;
}

inline void Geoset::setFaces(const Geoset::Faces &faces)
{
	this->m_faces = faces;
}

inline const Geoset::Faces& Geoset::faces() const
{
	return this->m_faces;
}

inline void Geoset::setGroupVertices(const Geoset::GroupVertices &groupVertices)
{
	this->m_groupVertices = groupVertices;
}

inline const Geoset::GroupVertices& Geoset::groupVertices() const
{
	return this->m_groupVertices;
}

inline void Geoset::setMatrices(const Geoset::Matrices &matrices)
{
	this->m_matrices = matrices;
}

inline const Geoset::Matrices& Geoset::matrices() const
{
	return this->m_matrices;
}

inline void Geoset::setMaterialId(long32 materialId)
{
	this->m_materialId = materialId;
}

inline long32 Geoset::materialId() const
{
	return this->m_materialId;
}

inline void Geoset::setSelectionGroup(long32 selectionGroup)
{
	this->m_selectionGroup = selectionGroup;
}

inline long32 Geoset::selectionGroup() const
{
	return this->m_selectionGroup;
}

inline void Geoset::setSelectable(Geoset::Selectable selectable)
{
	this->m_selectable = selectable;
}

inline Geoset::Selectable Geoset::selectable() const
{
	return this->m_selectable;
}

inline void Geoset::setGanimations(const Ganimations &ganimations)
{
	this->m_ganimations = ganimations;
}

inline const Geoset::Geoset::Ganimations& Geoset::ganimations() const
{
	return this->m_ganimations;
}

inline void Geoset::setTexturePatches(const Geoset::TexturePatches &texturePatches)
{
	this->m_texturePatches = texturePatches;
}

inline const Geoset::TexturePatches& Geoset::texturePatches() const
{
	return this->m_texturePatches;
}

inline void Geoset::setTextureVertices(const Geoset::TextureVertices &textureVertices)
{
	this->m_textureVertices = textureVertices;
}

inline const Geoset::TextureVertices& Geoset::textureVertices() const
{
	return this->m_textureVertices;
}

}

}

#endif
