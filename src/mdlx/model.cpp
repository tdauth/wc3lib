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
#include "../internationalisation.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

Model::Model(class Mdlx *mdlx) : MdxBlock("MODL", "Model", false), m_mdlx(mdlx)
{
}

Model::~Model()
{
}

std::streamsize Model::readMdl(istream &istream) throw (class Exception)
{
	string value;
	std::streamsize size = 0;
	parse(istream, value, size);

	if (value == "Model")
	{
		parse(istream, value, size);
		strcpy(this->m_name, value.c_str());
	}
	else if (value == "NumGeosets")
	{
	}
	else if (value == "NumGeosetAnims")
	{
	}
	else if (value == "NumHelpers")
	{
	}
	else if (value == "NumLights")
	{
	}
	else if (value == "NumBones")
	{
	}
	else if (value == "NumAttachments")
	{
	}
	/*
	NumGeosets <long>,
	NumGeosetAnims <long>,
	NumHelpers <long>,
	NumLights <long>,
	NumBones <long>,
	NumAttachments <long>,
	NumParticleEmitters <long>,
	NumParticleEmitters2 <long>,
	NumRibbonEmitters <long>,
	NumEvents <long>,
	BlendTime <long>,
	MinimumExtent { <float_x>, <float_y>, <float_z> },
	MaximumExtent { <float_x>, <float_y>, <float_z> },
	BoundsRadius <float>,
	*/


	return 0;
}

std::streamsize Model::writeMdl(ostream &ostream, const Mdlx *mdlx) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Model", this->name());

	if (mdlx != 0)
	{
		if (mdlx->geosets()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumGeosets", mdlx->geosets()->members().size(), 1);

		if (mdlx->geosetAnimations()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumGeosetAnims", mdlx->geosetAnimations()->members().size(), 1);

		if (mdlx->helpers()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumHelpers", mdlx->helpers()->members().size(), 1);

		if (mdlx->lights()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumLights", mdlx->lights()->members().size(), 1);

		if (mdlx->bones()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumBones", mdlx->bones()->members().size(), 1);

		if (mdlx->attachments()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumAttachments", mdlx->attachments()->members().size(), 1);

		if (mdlx->particleEmitters()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumParticleEmitters", mdlx->particleEmitters()->members().size(), 1);

		if (mdlx->particleEmitter2s()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumParticleEmitters2", mdlx->particleEmitter2s()->members().size(), 1);

		if (mdlx->ribbonEmitters()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumRibbonEmitters", mdlx->ribbonEmitters()->members().size(), 1);

		if (mdlx->events()->members().size() > 0)
			writeMdlValueProperty(ostream, size, "NumEvents", mdlx->events()->members().size(), 1);
	}

	size += Bounds::writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Model::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);
	long32 nbytes = 0;
	wc3lib::read(istream, nbytes, size);
	wc3lib::read(istream, this->m_name, size, nameSize);
	wc3lib::read(istream, this->m_unknown, size);
	size += Bounds::readMdx(istream);
	wc3lib::read(istream, this->m_blendTime, size);

	return size;
}

std::streamsize Model::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	wc3lib::write(ostream, this->name(), size, nameSize);
	wc3lib::write(ostream, this->unknown(), size);
	size += Bounds::writeMdx(ostream);
	wc3lib::write(ostream, this->blendTime(), size);

	writeByteCount(ostream, *reinterpret_cast<const long32*>(&size), position, size, false);

	return size;
}

}

}
