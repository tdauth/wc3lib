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

#include "ribbonemitter.hpp"
#include "mdlxtranslations.hpp"
#include "mdlxrotations.hpp"
#include "mdlxscalings.hpp"
#include "ribbonemittervisibilities.hpp"
#include "ribbonemitterheightsabove.hpp"
#include "ribbonemitterheightsbelow.hpp"
#include "../utilities.hpp"
#include "../vertex.hpp"

namespace wc3lib
{

namespace mdlx
{

RibbonEmitter::RibbonEmitter(class RibbonEmitters *ribbonEmitters) : Node(ribbonEmitters->mdlx()), GroupMdxBlockMember(ribbonEmitters, "RibbonEmitter"), m_visibilities(new RibbonEmitterVisibilities(this)), m_heightsAbove(new RibbonEmitterHeightsAbove(this)), m_heightsBelow(new RibbonEmitterHeightsBelow(this))
{
}

RibbonEmitter::~RibbonEmitter()
{
	delete this->m_visibilities;
	delete this->m_heightsAbove;
	delete this->m_heightsBelow;
}

std::streamsize RibbonEmitter::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize RibbonEmitter::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;

	writeMdlBlock(ostream, size, "RibbonEmitter", this->name(), 0, true);
	size += Node::writeMdl(ostream);

	if (!this->heightsAbove()->properties().empty())
		size += this->heightsAbove()->writeMdl(ostream);
	else
		writeMdlStaticValueProperty(ostream, size, "HeightAbove", this->heightAboveValue());

	if (!this->heightsBelow()->properties().empty())
		size += this->heightsBelow()->writeMdl(ostream);
	else
		writeMdlStaticValueProperty(ostream, size, "HeightBelow", this->heightBelowValue());


	writeMdlStaticVectorProperty(ostream, size, "Color", VertexData(colorRed(), colorGreen(), colorBlue()));
	writeMdlStaticValueProperty(ostream, size, "TextureSlot", this->unknown0());

	if (!this->visibilities()->properties().empty())
		size += this->visibilities()->writeMdl(ostream);
	else
		writeMdlStaticValueProperty(ostream, size, "Alpha", this->alpha());

	writeMdlValueProperty(ostream, size, "EmissionRate", this->emissionRate());
	writeMdlValueProperty(ostream, size, "LifeSpan", this->lifeSpan());

	if (this->gravity() != 0.0)
		writeMdlValueProperty(ostream, size, "Gravity", this->gravity());

	writeMdlValueProperty(ostream, size, "Rows", this->rows());
	writeMdlValueProperty(ostream, size, "Columns", this->columns());
	writeMdlValueProperty(ostream, size, "MaterialID", this->materialId());

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize RibbonEmitter::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	long32 nbytesi = 0;
	wc3lib::read(istream, nbytesi, size);
	size += Node::readMdx(istream);
	wc3lib::read(istream, this->m_heightAboveValue, size);
	wc3lib::read(istream, this->m_heightBelowValue, size);
	wc3lib::read(istream, this->m_alpha, size);
	wc3lib::read(istream, this->m_colorRed, size);
	wc3lib::read(istream, this->m_colorGreen, size);
	wc3lib::read(istream, this->m_colorBlue, size);
	wc3lib::read(istream, this->m_lifeSpan, size);
	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, this->m_emissionRate, size);
	wc3lib::read(istream, this->m_rows, size);
	wc3lib::read(istream, this->m_columns, size);
	wc3lib::read(istream, this->m_materialId, size);
	wc3lib::read(istream, this->m_gravity, size);
	size += this->m_visibilities->readMdx(istream);
	size += this->m_heightsAbove->readMdx(istream);
	size += this->m_heightsBelow->readMdx(istream);

	return size;
}

std::streamsize RibbonEmitter::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	std::streampos position;
	skipByteCount<long32>(ostream, position);
	size += Node::writeMdx(ostream);
	wc3lib::write(ostream, this->m_heightAboveValue, size);
	wc3lib::write(ostream, this->m_heightBelowValue, size);
	wc3lib::write(ostream, this->m_alpha, size);
	wc3lib::write(ostream, this->m_colorRed, size);
	wc3lib::write(ostream, this->m_colorGreen, size);
	wc3lib::write(ostream, this->m_colorBlue, size);
	wc3lib::write(ostream, this->m_lifeSpan, size);
	wc3lib::write(ostream, this->m_unknown0, size);
	wc3lib::write(ostream, this->m_emissionRate, size);
	wc3lib::write(ostream, this->m_rows, size);
	wc3lib::write(ostream, this->m_columns, size);
	wc3lib::write(ostream, this->m_materialId, size);
	wc3lib::write(ostream, this->m_gravity, size);
	size += this->m_visibilities->writeMdx(ostream);
	size += this->m_heightsAbove->writeMdx(ostream);
	size += this->m_heightsBelow->writeMdx(ostream);

	const long32 nbytesi = boost::numeric_cast<long32>(size);
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
