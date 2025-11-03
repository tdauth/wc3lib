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

#include <utility>

#include "geoset.hpp"

namespace wc3lib
{

namespace mdlx
{

Geoset::Geoset() : m_materialId(0),  m_selectionGroup(0),  m_selectable(Geoset::Selectable::None)
{
}

std::streamsize Geoset::read(InputStream &istream)
{
	clear();

	std::streamsize size = 0;
	long32 nbytes = 0; // exclusive size
	wc3lib::read(istream, nbytes, size);

	char8_t tag[MDX_TAG_SIZE];

	// VRTX
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"VRTX");
	long32 nvrts = 0;
	wc3lib::read(istream, nvrts, size);

	for (long32 i = 0; i < nvrts; i++) {
		VertexData vertex;
		size += vertex.read(istream);
		m_vertices.push_back(vertex);
	}

    // NRMS
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"NRMS");
	long32 nnrms = 0;
	wc3lib::read(istream, nnrms, size);

	for (long32 i = 0; i < nnrms; i++) {
		VertexData normal;
		size += normal.read(istream);
		m_normals.push_back(normal);
	}

    // PTYP
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"PTYP");
	long32 nptyps = 0;
	wc3lib::read(istream, nptyps, size);
	std::vector<wc3lib::mdlx::Faces::Type> primitiveTypes;
	primitiveTypes.reserve(nptyps);

	for (long32 i = 0; i < nptyps; i++) {
		long32 t = 0;
		wc3lib::read(istream, t, size);
		wc3lib::mdlx::Faces::Type primitiveType = wc3lib::mdlx::Faces::Type::Triangles;

		if (t != std::to_underlying(wc3lib::mdlx::Faces::Type::Triangles)) {
			std::cerr << "Unknown primitive type " << t << std::endl;
		} else {
			primitiveType = static_cast<wc3lib::mdlx::Faces::Type>(t);
		}

		primitiveTypes.push_back(primitiveType);
	}

    // PCNT
    wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"PCNT");
	long32 npcnts = 0;
	wc3lib::read(istream, npcnts, size);
	std::vector<long32> primCounts;
	primCounts.reserve(npcnts);

	for (long32 i = 0; i < npcnts; i++) {
		long32 c = 0;
		wc3lib::read(istream, c, size);
		primCounts.push_back(c);
	}

	// PVTX
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"PVTX");
	long32 ntris = 0;
	wc3lib::read(istream, ntris, size);
	std::vector<short16> triangles(ntris);
	wc3lib::readVector(istream, triangles, size);

	// Building faces from PTYP, PCNT and PVTX
	std::size_t index = 0;
    std::size_t primitiveVertexIndex = 0;

	for (wc3lib::mdlx::Faces::Type t : primitiveTypes) {
        switch (t) {
            case wc3lib::mdlx::Faces::Type::Triangles: {
                long32 count = primCounts[index];
                wc3lib::mdlx::Faces::Vertices v;
                v.reserve(count);

                for (long32 x = 0; x < count; x++) {
                    v.push_back(triangles[primitiveVertexIndex]);
                    primitiveVertexIndex++;
                }

                wc3lib::mdlx::Faces f;
                f.setType(t);
                f.setVertices(v);
                m_faces.push_back(f);


                break;
            }

            default: {
                std::cerr << "Unknown primitive type " << std::to_underlying(t) << std::endl;
            }
        }

        index++;
    }

	// GNDX
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"GNDX");
	long32 nvgrps = 0;
	wc3lib::read(istream, nvgrps, size);
	m_groupVertices = GroupVertices(nvgrps);
	wc3lib::readVector(istream, m_groupVertices, size);

	// MTGC
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"MTGC");
	long32 nmtrcs = 0;
	wc3lib::read(istream, nmtrcs, size);
	std::vector<long32> groupCounts(nmtrcs);
	wc3lib::readVector(istream, groupCounts, size);

	// MATS
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"MATS");
	long32 nmats = 0;
	wc3lib::read(istream, nmats, size);
	std::vector<long32> matrices(nmats);
	wc3lib::readVector(istream, matrices, size);

	// Building matrices from GNDX, MTGC and MATS
	m_matrices = Matrices(nmats);

	for (std::size_t i = 0; i < nmats; i++)
	{
		Matrix matrix;

		m_matrices.push_back(std::move(matrix));
	}

    // TODO Support all blocks.

    size += Bounds::read(istream);

    skipMdxInclusiveEmptyBytes(istream, nbytes, nbytes + sizeof(nbytes) - size);

    return size;
}

std::streamsize Geoset::write(OutputStream &ostream) const
{
    std::streamsize size = 0;
    auto p = ostream.tellp();
    ostream.seekp(sizeof(long32), std::ios::cur);
    //wc3lib::read(nbytes, size);

    // VRTX
    writeMdxTag(ostream, u8"VRTX", size);

    for (const VertexData &vertex : vertices()) {
        size += vertex.write(ostream);
    }

    // NRMS
    writeMdxTag(ostream, u8"NRMS", size);

    for (const VertexData &normal : normals()) {
        size += normal.write(ostream);
    }

    // TODO Support all GEOS types here.

    size += Bounds::write(ostream);

    // write exclusive size
    auto p2 = ostream.tellp();
    ostream.seekp(p);
    wc3lib::write(ostream, size - sizeof(long32), size);
    ostream.seekp(p2);

    return size;
}

void Geoset::clear()
{
	m_vertices.clear();
	m_normals.clear();
	m_faces.clear();
	m_matrices.clear();
}

}

}
