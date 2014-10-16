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

#include <boost/format.hpp>

#include "model.hpp"
/// @todo Maybe we should one single "objects" include file.
#include "mdlx.hpp"
#include "geosets.hpp"
#include "geosetanimations.hpp"
#include "helpers.hpp"
#include "lights.hpp"
#include "bones.hpp"
#include "attachments.hpp"
#include "particleemitters.hpp"
#include "particleemitter2s.hpp"
#include "ribbonemitters.hpp"
#include "events.hpp"
#include "../i18n.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

Model::Model() : MdxBlock("MODL", "Model", false)
{
}

Model::~Model()
{
}

std::streamsize Model::readMdl(istream &istream)
{
	return 0;
}

std::streamsize Model::writeMdl(ostream &ostream, const Mdlx *mdlx) const
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Model", this->name(), 0, true);

	if (mdlx != 0)
	{
		if (mdlx->geosets()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumGeosets", mdlx->geosets()->members().size(), 1);
		}

		if (mdlx->geosetAnimations()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumGeosetAnims", mdlx->geosetAnimations()->members().size(), 1);
		}

		if (mdlx->helpers()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumHelpers", mdlx->helpers()->members().size(), 1);
		}

		if (mdlx->lights()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumLights", mdlx->lights()->members().size(), 1);
		}

		if (mdlx->bones()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumBones", mdlx->bones()->members().size(), 1);
		}

		if (mdlx->attachments()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumAttachments", mdlx->attachments()->members().size(), 1);
		}

		if (mdlx->particleEmitters()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumParticleEmitters", mdlx->particleEmitters()->members().size(), 1);
		}

		if (mdlx->particleEmitter2s()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumParticleEmitters2", mdlx->particleEmitter2s()->members().size(), 1);
		}

		if (mdlx->ribbonEmitters()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumRibbonEmitters", mdlx->ribbonEmitters()->members().size(), 1);
		}

		if (mdlx->events()->members().size() > 0)
		{
			writeMdlValueProperty(ostream, size, "NumEvents", mdlx->events()->members().size(), 1);
		}
	}

	writeMdlValueProperty(ostream, size, "BlendTime", this->blendTime());

	size += this->bounds().writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Model::readMdx(istream &istream)
{
	std::streamsize size = MdxBlock::readMdx(istream);
	long32 nbytes = 0;
	wc3lib::read(istream, nbytes, size);
	wc3lib::read(istream, this->m_name, size, nameSize);
	wc3lib::read(istream, this->m_unknown, size);
	size += this->m_bounds.readMdx(istream);
	wc3lib::read(istream, this->m_blendTime, size);

	return size;
}

std::streamsize Model::writeMdx(ostream &ostream) const
{
	std::streamsize size = MdxBlock::writeMdx(ostream);
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	wc3lib::write(ostream, this->name(), size, nameSize);
	wc3lib::write(ostream, this->unknown(), size);
	size += this->bounds().writeMdx(ostream);
	wc3lib::write(ostream, this->blendTime(), size);

	const long32 nbytes = boost::numeric_cast<long32>(size - mdxIdentifierSize);
	writeByteCount(ostream, nbytes, position, size, false);

	return size;
}

}

}
