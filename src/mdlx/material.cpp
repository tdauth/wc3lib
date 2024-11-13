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

#include "material.hpp"
#include "mdlx.hpp"

namespace wc3lib
{

namespace mdlx
{

Material::Material() : m_priorityPlane(0), m_renderMode(RenderMode::ConstantColor)
{
}

std::streamsize Material::read(InputStream &istream, long32 version)
{
	std::streamsize size = 0;
	long32 inclusiveSize = 0;
	wc3lib::read(istream, inclusiveSize, size);
	wc3lib::read(istream, m_priorityPlane, size);
	wc3lib::read(istream, m_renderMode, size);

	if (version > 800)
	{
		wc3lib::read(istream, m_shader, size, sizeof(byte) * shaderSize);
	}

	// LAYS
	readMdxTag(istream, u8"LAYS", size);
	long32 layersCount = 0;
	wc3lib::read(istream, layersCount, size);
	m_layers.clear();

	for (long32 i = 0; i < layersCount; i++)
	{
		Layer layer;
		size += layer.read(istream, version);
		m_layers.push_back(layer);
	}

	skipMdxInclusiveEmptyBytes(istream, inclusiveSize, size);

	return size;
}

std::streamsize Material::write(OutputStream &ostream, long32 version) const
{
	std::streamsize size = 0;
	auto p = ostream.tellp();
	wc3lib::write(ostream, m_priorityPlane, size);
	wc3lib::write(ostream, m_renderMode, size);

	if (version > 800)
	{
		wc3lib::write(ostream, m_shader, size, sizeof(byte) * shaderSize);
	}

	writeMdxTag(ostream, u8"LAYS", size);
	const long32 layersCount = m_layers.size();
	wc3lib::write(ostream, layersCount, size);

	for (const auto &layer : m_layers)
	{
		size += layer.write(ostream, version);
	}

	long32 inclusiveSize = size + sizeof(long32);
	auto p2 = ostream.tellp();
	ostream.seekp(p);
	wc3lib::write(ostream, inclusiveSize, size);
	ostream.seekp(p2);

	return size;
}

std::streamsize Material::read(InputStream &istream)
{
	return read(istream, Mdlx::currentVersion);
}

std::streamsize Material::write(OutputStream &ostream) const
{
	return write(ostream, Mdlx::currentVersion);
}

}

}

