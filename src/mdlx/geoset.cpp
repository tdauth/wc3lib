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

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/cast.hpp>

#include "geoset.hpp"
#include "geosets.hpp"
#include "vertices.hpp"
#include "normals.hpp"
#include "primitivetypes.hpp"
#include "primitivesizes.hpp"
#include "primitivevertices.hpp"
#include "groupvertices.hpp"
#include "matrixgroupcounts.hpp"
#include "matrices.hpp"
#include "ganimation.hpp"
#include "texturepatches.hpp"
#include "texturevertices.hpp"
#include "../internationalisation.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

Geoset::Geoset(class Geosets *geosets) : GroupMdxBlockMember(geosets, "Geoset"), m_vertices(new Vertices(this)), m_normals(new Normals(this)),  m_primitveTypes(new PrimitiveTypes(this)), m_primitiveSizes(new PrimitiveSizes(this)),  m_primitiveVertices(new PrimitiveVertices(this)),  m_groupVertices(new GroupVertices(this)), m_matrixGroupCounts(new MatrixGroupCounts(this)),  m_matrices(new Matrices(this)), m_materialId(0),  m_selectionGroup(0),  m_selectable(Geoset::Selectable::None), m_texturePatches(new TexturePatches(this)), m_textureVertices(new TextureVertices(this))
{
}

Geoset::~Geoset()
{
	delete this->m_vertices;
	delete this->m_normals;
	delete this->m_primitveTypes;
	delete this->m_primitiveSizes;
	delete this->m_primitiveVertices;
	delete this->m_groupVertices;
	delete this->m_matrixGroupCounts;
	delete this->m_matrices;
	delete this->m_texturePatches;
	delete this->m_textureVertices;
}

std::streamsize Geoset::readMdl(istream &istream) throw (class Exception)
{
	throw Exception(_("Geoset::readMdl: Not implemented yet."));
}

std::streamsize Geoset::writeMdl(ostream &ostream) const throw (class Exception)
{
	throw Exception(_("Geoset::writeMdl: Not implemented yet."));
}

std::streamsize Geoset::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	long32 nbytesi; // Magos specification says including byte count!
	wc3lib::read(istream, nbytesi, size);

	size += this->m_vertices->readMdx(istream);
	size += this->m_normals->readMdx(istream);
	size += this->m_primitveTypes->readMdx(istream);
	size += this->m_primitiveSizes->readMdx(istream);
	size += this->m_primitiveVertices->readMdx(istream);
	size += this->m_groupVertices->readMdx(istream);
	size += this->m_matrixGroupCounts->readMdx(istream);
	size += this->m_matrices->readMdx(istream);
	wc3lib::read(istream, this->m_materialId, size);
	wc3lib::read(istream, this->m_selectionGroup, size);
	wc3lib::read(istream, this->m_selectable, size);
	size += Bounds::readMdx(istream);
	long32 geosetAnimationNumber;
	wc3lib::read(istream, geosetAnimationNumber, size);
	ganimations().reserve(geosetAnimationNumber);

	for ( ; geosetAnimationNumber > 0; --geosetAnimationNumber)
	{
		Ganimation *ganimation = new Ganimation(this);
		size += ganimation->readMdx(istream);
		this->ganimations().push_back(ganimation);
	}

	size += this->m_texturePatches->readMdx(istream);
	size += this->m_textureVertices->readMdx(istream);

	if (nbytesi != size)
		throw Exception(boost::format(_("Geoset: Expected byte count %1% is not equal to read size %2%.")) % nbytesi % size);

	return size;
}

std::streamsize Geoset::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize size = 0;
	size += this->m_vertices->writeMdx(ostream);
	size += this->m_normals->writeMdx(ostream);
	size += this->m_primitveTypes->writeMdx(ostream);
	size += this->m_primitiveSizes->writeMdx(ostream);
	size += this->m_primitiveVertices->writeMdx(ostream);
	size += this->m_groupVertices->writeMdx(ostream);
	size += this->m_matrixGroupCounts->writeMdx(ostream);
	size += this->m_matrices->writeMdx(ostream);
	wc3lib::write(ostream, this->m_materialId, size);
	wc3lib::write(ostream, this->m_selectionGroup, size);
	wc3lib::write(ostream, this->m_selectable, size);
	size += Bounds::writeMdx(ostream);
	wc3lib::write(ostream, static_cast<const long32>(this->ganimations().size()), size);

	BOOST_FOREACH(Ganimations::const_reference ganimation, this->ganimations())
		size += ganimation.writeMdx(ostream);

	size += this->m_texturePatches->writeMdx(ostream);
	size += this->m_textureVertices->writeMdx(ostream);

	long32 nbytesi = size; // Magos specification says including byte count!
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
