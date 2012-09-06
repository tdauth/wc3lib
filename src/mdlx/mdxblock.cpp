/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.de                                                      *
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

#include "mdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

MdxBlock::MdxBlock(const byte mdxIdentifier[MdxBlock::mdxIdentifierSize], const string &mdlKeyword, bool optional) : m_mdlKeyword(mdlKeyword), m_optional(optional), m_exists(false)
{
	memcpy(reinterpret_cast<void*>(const_cast<byte*>(this->m_mdxIdentifier)), reinterpret_cast<const void*>(mdxIdentifier), MdxBlock::mdxIdentifierSize);
}

MdxBlock::~MdxBlock()
{
}

/// @todo Consider optional like in Python script.
std::streamsize MdxBlock::readMdx(istream &istream) throw (class Exception)
{
	const std::streampos requiredPosition = istream.tellg() + (std::streampos)MdxBlock::mdxIdentifierSize;
	const std::streampos end = endPosition(istream);

	// not enough space in stream
	if (end < requiredPosition)
	{
		if (optional())
			return 0;
		else
			throw Exception(boost::format(_("Input stream hasn't enough space. Missing %1% bytes.")) % (requiredPosition - end));
	}

	std::streamsize size = 0;
	byte identifier[MdxBlock::mdxIdentifierSize];
	const istream::pos_type position = istream.tellg();
	wc3lib::read(istream, identifier[0], size, MdxBlock::mdxIdentifierSize);

	if (memcmp(identifier, mdxIdentifier(), MdxBlock::mdxIdentifierSize) != 0)
	{
		if (this->optional())
		{
			istream.seekg(position);
			std::cout << boost::format(_("Block %1% is optional and doesn't exist.\nIt is not equal to identifier \"%2%\".")) % mdxIdentifier() % identifier << std::endl;

			return 0;
		}
		else
			throw Exception(boost::format(_("Unexptected identifier \"%s\". Missing \"%s\" block name.")) % identifier % mdxIdentifier());
	}

	this->m_exists = true;
	std::cout << boost::format(_("Block: %1%")) % mdxIdentifier() << std::endl;

	return size;
}

std::streamsize MdxBlock::writeMdx(ostream &ostream) const throw (class Exception)
{
	if (!this->exists())
		return 0;

	std::streamsize size = 0;
	wc3lib::write(ostream, mdxIdentifier()[0], size, MdxBlock::mdxIdentifierSize);

	return size;
}

std::streamsize MdxBlock::readMdl(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	string name;
	parseMdlNamedBlock(istream, mdlKeyword(), name, size, optional());

	return size;
}

std::streamsize MdxBlock::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

bool MdxBlock::moveToMdxIdentifier(istream &istream) const
{
	byte readBlockName[MdxBlock::mdxIdentifierSize];

	while (istream)
	{
		istream.read(readBlockName, sizeof(readBlockName));

		if (memcmp(readBlockName, mdxIdentifier(), sizeof(mdxIdentifier())) == 0)
			return true;
	}

	return true;
}

}

}
