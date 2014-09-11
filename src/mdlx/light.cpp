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

#include "light.hpp"
#include "lights.hpp"
#include "intensities.hpp"
#include "lightambientvisibilities.hpp"
#include "lightambientcolors.hpp"
#include "ambientcolors.hpp"
#include "ambientintensities.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

Light::Light(class Lights *lights) : Object(lights->mdlx()), GroupMdxBlockMember(lights, "Light"), m_intensities(new Intensities(this)), m_visibilities(new LightAmbientVisibilities(this)), m_colors(new LightAmbientColors(this)), m_ambientColors(new AmbientColors(this)), m_ambientIntensities(new AmbientIntensities(this))
{
}

Light::~Light()
{
	delete this->m_intensities;
	delete this->m_visibilities;
	delete this->m_colors;
	delete this->m_ambientColors;
	delete this->m_ambientIntensities;
}

std::streamsize Light::readMdl(istream &istream)
{
	return 0;
}

std::streamsize Light::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Light", this->name(), 0, true);

	size += Object::writeMdl(ostream);

	writeMdlStaticValueProperty(ostream, size, "AttenuationStart", this->attenuationStart());
	writeMdlStaticValueProperty(ostream, size, "AttenuationEnd", this->attenuationEnd());

	if (intensities()->properties().empty())
		writeMdlStaticValueProperty(ostream, size, "Intensity", this->intensity());
	else
		size += intensities()->writeMdl(ostream);

	if (colors()->properties().empty())
		writeMdlStaticVectorProperty(ostream, size, "Color", this->color());
	else
		size += colors()->writeMdl(ostream);

	if (ambientIntensities()->properties().empty())
		writeMdlStaticValueProperty(ostream, size, "AmbIntensity", this->ambientIntensity());
	else
		size += ambientIntensities()->writeMdl(ostream);

	if (ambientColors()->properties().empty())
		writeMdlStaticVectorProperty(ostream, size, "AmbColor", this->ambientColor());
	else
		size += ambientColors()->writeMdl(ostream);

	size += visibilities()->writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Light::readMdx(istream &istream)
{
	long32 nbytesi;
	std::streamsize size = 0;
	wc3lib::read(istream, nbytesi, size);
	size += Object::readMdx(istream);
	wc3lib::read<long32>(istream, reinterpret_cast<long32&>(this->m_type), size);
	wc3lib::read(istream, this->m_attenuationStart, size);
	wc3lib::read(istream, this->m_attenuationEnd, size);
	size += this->color().read(istream);
	wc3lib::read(istream, this->m_intensity, size);
	size += this->ambientColor().read(istream);
	wc3lib::read(istream, this->m_ambientIntensity, size);
	size += this->m_intensities->readMdx(istream);
	size += this->m_visibilities->readMdx(istream);
	size += this->m_colors->readMdx(istream);
	size += this->m_ambientColors->readMdx(istream);
	size += this->m_ambientIntensities->readMdx(istream);

	if (nbytesi != size)
		throw Exception(boost::format(_("Light: Expected byte count %1% is not equal to read size %2%.")) % nbytesi % size);

	return size;
}

std::streamsize Light::writeMdx(std::ostream &ostream) const
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize size = 0;
	size += Object::writeMdx(ostream);
	wc3lib::write(ostream, this->type(), size);
	wc3lib::write(ostream, this->attenuationStart(), size);
	wc3lib::write(ostream, this->attenuationEnd(), size);
	size += this->color().write(ostream);
	wc3lib::write(ostream, this->intensity(), size);
	size += this->ambientColor().write(ostream);
	wc3lib::write(ostream, this->ambientIntensity(), size);
	size += this->m_intensities->writeMdx(ostream);
	size += this->m_visibilities->writeMdx(ostream);
	size += this->m_colors->writeMdx(ostream);
	size += this->m_ambientColors->writeMdx(ostream);
	size += this->m_ambientIntensities->writeMdx(ostream);

	writeByteCount(ostream, boost::numeric_cast<long32>(size), position, size, true);

	return size;
}

}

}
