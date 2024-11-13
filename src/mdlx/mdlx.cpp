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

#include <cstring>

#include "mdlx.hpp"

namespace wc3lib
{

namespace mdlx
{

const long32 Mdlx::currentVersion = 800;

Mdlx::Mdlx() : m_modelVersion(currentVersion)
{
}

Mdlx::~Mdlx()
{
}

std::streamsize Mdlx::read(InputStream &istream)
{
	std::streamsize size = 0;
	char8_t tag[MDX_TAG_SIZE];
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"MDLX");

	// VERS
	MdxHeader versionHeader = readMdxHeader(istream, size, u8"VERS");
	std::streamsize modelVersionSize = 0;
	wc3lib::read(istream, m_modelVersion, modelVersionSize);
	skipMdxHeaderEmptyBytes(istream, versionHeader, modelVersionSize);
	size += modelVersionSize;

	// MODL
	size += m_model.read(istream);

	// SEQS
	m_sequences.clear();
	MdxHeader sequencesHeader = readMdxHeader(istream, size, u8"SEQS");

	while (sequencesHeader.size > 0)
	{
		Sequence sequence;
		const std::streamsize s = sequence.read(istream);
		sequencesHeader.size -= s;
		size += s;
		m_sequences.push_back(sequence);
	}

	return size;
}

std::streamsize Mdlx::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, "MDLX", size, sizeof(char8_t) * 4);

	// VERS
	writeMdxHeader(ostream, size, u8"VERS", sizeof(m_modelVersion));
	wc3lib::write(ostream, m_modelVersion, size);

	// MODL
	size += m_model.write(ostream);

	// SEQS TODO Implement

	return size;
}

}

}
