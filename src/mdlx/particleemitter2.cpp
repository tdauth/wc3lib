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

#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include "particleemitter2.hpp"
#include "mdlxtranslations.hpp"
#include "mdlxrotations.hpp"
#include "mdlxscalings.hpp"
#include "segmentcolor.hpp"
#include "particleemitter2speeds.hpp"
#include "particleemitter2latitudes.hpp"
#include "emissionrates.hpp"
#include "particleemitter2visibilities.hpp"
#include "lengths.hpp"
#include "particleemitter2widths.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

ParticleEmitter2::ParticleEmitter2(class ParticleEmitter2s *particleEmitters) : Node(particleEmitters->mdlx()), GroupMdxBlockMember(particleEmitters, "ParticleEmitter2"), m_speeds(new ParticleEmitter2Speeds(this)), m_latitudes(new ParticleEmitter2Latitudes(this)), m_emissionRates(new EmissionRates(particleEmitters->mdlx())), m_visibilities(new ParticleEmitter2Visibilities(this)), m_numbers(new Lengths(this)), m_widths(new ParticleEmitter2Widths(this))
{
}

ParticleEmitter2::~ParticleEmitter2()
{
	delete this->m_speeds;
	delete this->m_latitudes;
	delete this->m_emissionRates;
	delete this->m_visibilities;
	delete this->m_numbers;
	delete this->m_widths;
}

std::streamsize ParticleEmitter2::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize ParticleEmitter2::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize ParticleEmitter2::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	long32 nbytesi;
	wc3lib::read(istream, nbytesi, size);
	size += Node::readMdx(istream);
	wc3lib::read(istream, this->m_speed, size);
	wc3lib::read(istream, this->m_variation, size);
	wc3lib::read(istream, this->m_latitidue, size);
	wc3lib::read(istream, this->m_gravity, size);
	wc3lib::read(istream, this->m_lifespan, size);
	wc3lib::read(istream, this->m_emissionRate, size);
	wc3lib::read(istream, this->m_length, size);
	wc3lib::read(istream, this->m_width, size);
	wc3lib::read<long32>(istream, (long32&)this->m_filterMode, size);
	wc3lib::read(istream, this->m_rows, size);
	wc3lib::read(istream, this->m_columns, size);
	wc3lib::read<long32>(istream, (long32&)this->m_flags, size);
	wc3lib::read(istream, this->m_tailLength, size);
	wc3lib::read(istream, this->m_time, size);

	for (std::size_t i = 0; i < segmentColorsSize; ++i)
	{
		this->segmentColors().replace(i, new SegmentColor(this));
		size += this->segmentColors()[i].readMdx(istream);
	}

	wc3lib::read(istream, this->m_alpha1, size);
	wc3lib::read(istream, this->m_alpha2, size);
	wc3lib::read(istream, this->m_alpha3, size);
	wc3lib::read(istream, this->m_scalingX, size);
	wc3lib::read(istream, this->m_scalingY, size);
	wc3lib::read(istream, this->m_scalingZ, size);
	wc3lib::read(istream, this->m_lifeSpanUvAnim1, size);
	wc3lib::read(istream, this->m_lifeSpanUvAnim2, size);
	wc3lib::read(istream, this->m_lifeSpanUvAnim3, size);
	wc3lib::read(istream, this->m_decayUvAnim1, size);
	wc3lib::read(istream, this->m_decayUvAnim2, size);
	wc3lib::read(istream, this->m_decayUvAnim3, size);
	wc3lib::read(istream, this->m_tailUvAnim1, size);
	wc3lib::read(istream, this->m_tailUvAnim2, size);
	wc3lib::read(istream, this->m_tailUvAnim3, size);
	wc3lib::read(istream, this->m_tailDecayUvAnim1, size);
	wc3lib::read(istream, this->m_tailDecayUvAnim2, size);
	wc3lib::read(istream, this->m_tailDecayUvAnim3, size);
	wc3lib::read(istream, this->m_textureId, size);
	wc3lib::read(istream, this->m_squirt, size);
	wc3lib::read(istream, this->m_priorityPlane, size);
	wc3lib::read(istream, *reinterpret_cast<long32*>(&this->m_replaceableId), size);
	size += this->m_speeds->readMdx(istream);
	size += this->m_latitudes->readMdx(istream);
	size += this->m_emissionRates->readMdx(istream);
	size += this->m_visibilities->readMdx(istream);
	size += this->m_numbers->readMdx(istream);
	size += this->m_widths->readMdx(istream);

	return size;
}

std::streamsize ParticleEmitter2::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<wc3lib::ostream>(ostream, position);

	std::streamsize size = Node::writeMdx(ostream);
	wc3lib::write(ostream, this->m_speed, size);
	wc3lib::write(ostream, this->m_variation, size);
	wc3lib::write(ostream, this->m_latitidue, size);
	wc3lib::write(ostream, this->m_gravity, size);
	wc3lib::write(ostream, this->m_lifespan, size);
	wc3lib::write(ostream, this->m_emissionRate, size);
	wc3lib::write(ostream, this->m_length, size);
	wc3lib::write(ostream, this->m_width, size);
	wc3lib::write<long32>(ostream, this->m_filterMode, size);
	wc3lib::write(ostream, this->m_rows, size);
	wc3lib::write(ostream, this->m_columns, size);
	wc3lib::write<long32>(ostream, this->m_flags, size);
	wc3lib::write(ostream, this->m_tailLength, size);
	wc3lib::write(ostream, this->m_time, size);

	BOOST_FOREACH(SegmentColors::const_reference color, this->segmentColors())
		size += color.writeMdx(ostream);

	wc3lib::write(ostream, this->m_alpha1, size);
	wc3lib::write(ostream, this->m_alpha2, size);
	wc3lib::write(ostream, this->m_alpha3, size);
	wc3lib::write(ostream, this->m_scalingX, size);
	wc3lib::write(ostream, this->m_scalingY, size);
	wc3lib::write(ostream, this->m_scalingZ, size);
	wc3lib::write(ostream, this->m_lifeSpanUvAnim1, size);
	wc3lib::write(ostream, this->m_lifeSpanUvAnim2, size);
	wc3lib::write(ostream, this->m_lifeSpanUvAnim3, size);
	wc3lib::write(ostream, this->m_decayUvAnim1, size);
	wc3lib::write(ostream, this->m_decayUvAnim2, size);
	wc3lib::write(ostream, this->m_decayUvAnim3, size);
	wc3lib::write(ostream, this->m_tailUvAnim1, size);
	wc3lib::write(ostream, this->m_tailUvAnim2, size);
	wc3lib::write(ostream, this->m_tailUvAnim3, size);
	wc3lib::write(ostream, this->m_tailDecayUvAnim1, size);
	wc3lib::write(ostream, this->m_tailDecayUvAnim2, size);
	wc3lib::write(ostream, this->m_tailDecayUvAnim3, size);
	wc3lib::write(ostream, this->m_textureId, size);
	wc3lib::write(ostream, this->m_squirt, size);
	wc3lib::write(ostream, this->m_priorityPlane, size);
	wc3lib::write<long32>(ostream, this->m_replaceableId, size);
	size += this->m_speeds->writeMdx(ostream);
	size += this->m_latitudes->writeMdx(ostream);
	size += this->m_emissionRates->writeMdx(ostream);
	size += this->m_visibilities->writeMdx(ostream);
	size += this->m_numbers->writeMdx(ostream);
	size += this->m_widths->writeMdx(ostream);

	long32 nbytesi = size;
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
