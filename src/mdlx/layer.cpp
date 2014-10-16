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

#include "layer.hpp"
#include "material.hpp"
#include "materials.hpp"
#include "materialalphas.hpp"
#include "textureids.hpp"

namespace wc3lib
{

namespace mdlx
{

Layer::Layer(class Layers *layers) : GroupMdxBlockMember(layers, "Layer"), m_alphas(new MaterialAlphas(this)), m_textureIds(new TextureIds(this->layers()->material()->materials()->mdlx()))
{
}

Layer::~Layer()
{
	delete this->m_alphas;
	delete this->m_textureIds;
}

std::streamsize Layer::readMdl(istream &istream)
{
	return 0;
}

std::streamsize Layer::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Layer");

	writeMdlValueProperty(ostream, size, "FilterMode", filterMode(this->filterMode()));

	if (this->shading() & Shading::Unshaded)
	{
		writeMdlProperty(ostream, size, "Unshaded");
	}

	if (this->shading() & Shading::SphereEnvironmentMap)
	{
		writeMdlProperty(ostream, size, "SphereEnvironmentMap");
	}

	if (this->shading() & Shading::TwoSided)
	{
		writeMdlProperty(ostream, size, "TwoSided");
	}

	if (this->shading() & Shading::Unfogged)
	{
		writeMdlProperty(ostream, size, "Unfogged");
	}

	if (this->shading() & Shading::NoDepthTest)
	{
		writeMdlProperty(ostream, size, "NoDepthTest");
	}

	if (this->shading() & Shading::NoDepthSet)
	{
		writeMdlProperty(ostream, size, "NoDepthSet");
	}

	if (this->textureIds()->properties().empty())
	{
		writeMdlStaticValueProperty(ostream, size, "TextureID", this->textureId());
	}
	else
	{
		size += this->textureIds()->writeMdl(ostream);
	}

	if (tvertexAnimationId() != noneId)
	{
		writeMdlValueProperty(ostream, size, "TVertexAnimId", this->tvertexAnimationId());
	}

	// If CoordId for any of the Layers in a Material is nonzero, CoordId appears
	// NOTE War3ModelEditor cannot recognize CoordId
	bool appears = false;

	BOOST_FOREACH(Layers::Members::const_reference ref, this->layers()->members())
	{
		if (boost::polymorphic_cast<const Layer*>(&ref)->coordinatesId() != 0)
		{
			appears = true;

			break;
		}
	}

	if (appears)
	{
		writeMdlValueProperty(ostream, size, "CoordId", this->coordinatesId());
	}

	if (this->alphas()->properties().empty())
	{
		writeMdlStaticValueProperty(ostream, size, "Alpha", this->alpha());
	}
	else
	{
		size += this->alphas()->writeMdl(ostream);
	}

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Layer::readMdx(istream &istream)
{
	std::streamsize size = 0;
	long32 nbytesi = 0;
	wc3lib::read(istream, nbytesi, size);
	wc3lib::read<long32>(istream, *reinterpret_cast<long32*>(&this->m_filterMode), size);
	wc3lib::read<long32>(istream, *reinterpret_cast<long32*>(&this->m_shading), size);
	wc3lib::read(istream, this->m_textureId, size);
	wc3lib::read(istream, this->m_tvertexAnimationId, size);
	wc3lib::read(istream, this->m_coordinatesId, size);
	wc3lib::read(istream, this->m_alpha, size);
	size += this->alphas()->readMdx(istream);
	size += this->textureIds()->readMdx(istream);

	return size;
}

std::streamsize Layer::writeMdx(ostream &ostream) const
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize size = 0;
	wc3lib::write(ostream, (long32)(this->filterMode()), size);
	wc3lib::write(ostream, (long32)(this->shading()), size);
	wc3lib::write(ostream, this->textureId(), size);
	wc3lib::write(ostream, this->tvertexAnimationId(), size);
	wc3lib::write(ostream, this->coordinatesId(), size);
	wc3lib::write(ostream, this->alpha(), size);
	size += this->alphas()->writeMdx(ostream);
	size += this->textureIds()->writeMdx(ostream);

	long32 nbytesi = boost::numeric_cast<long32>(size);
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
