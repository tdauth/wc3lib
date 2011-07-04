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

std::streamsize Model::writeMdl(ostream &ostream) const throw (class Exception)
{
	ostream << "Model \"" << this->m_name << "\" {\n";

/*
	mdlx stuff
	if (this->mdlx()->geosets()->geosets().size() > 0)
		ostream << "\tNumGeosets " << this->mdlx()->geosets()->geosets().size() << ",\n";

	if (this->mdlx()->geosetAnimations()->geosetAnimations().size() > 0)
		ostream << "\tNumGeosetAnims " << this->mdlx()->geosetAnimations()->geosetAnimations().size() << ",\n";

	if (this->mdlx()->helpers()->helpers().size() > 0)
		ostream << "\tNumHelpers " << this->mdlx()->helpers()->helpers().size() << ",\n";

	if (this->mdlx()->lights()->lights().size() > 0)
		ostream << "\tNumLights " << this->lights()->lights().size() << ",\n";

	if (this->mdlx()->bones()->bones().size() > 0)
		ostream << "\tNumBones " << this->mdlx()->bones()->bones().size() << ",\n";

	if (this->mdlx()->attachments()->attachments().size() > 0)
		ostream << "\tNumAttachements " << this->mdlx()->attachments()->attachments().size() << ",\n";

	if (this->mdlx()->particleEmitters()->particleEmitters().size() > 0)
		ostream << "\tNumParticleEmitters " << this->mdlx()->particleEmitters()->particleEmitters().size() << ",\n";

	if (this->mdlx()->particleEmitter2s()->particleEmitters().size() > 0)
		ostream << "\tNumParticleEmitters2 " << this->mdlx()->particleEmitter2s()->particleEmitters().size() << ",\n";

	if (this->mdlx()->ribbonEmitters->ribbonEmitters.size() > 0)
		ostream << "\tNumRibbonEmitters " << this->mdlx()->ribbonEmitters->ribbonEmitters.size() << ",\n";

	if (this->mdlx()->events()->events().size() > 0)
		ostream << "\tNumEvents " << this->mdlx()->events()->events().size() << ",\n";
*/

	Bounds::writeMdl(ostream);

	ostream << "}\n";

	return 0;
}

std::streamsize Model::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);
	long32 nbytes = 0;
	wc3lib::read(istream, nbytes, size);
	wc3lib::read(istream, this->m_name, size);
	wc3lib::read(istream, this->m_animationFileName, size);
	size += Bounds::readMdx(istream);
	wc3lib::read(istream, this->m_blendTime, size);

	return size;
}

std::streamsize Model::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	wc3lib::write(ostream, this->m_name, size);
	wc3lib::write(ostream, this->m_animationFileName, size);
	size += Bounds::writeMdx(ostream);
	wc3lib::write(ostream, this->m_blendTime, size);

	writeByteCount(ostream, *reinterpret_cast<const long32*>(&size), position, size, false);

	return size;
}

}

}
