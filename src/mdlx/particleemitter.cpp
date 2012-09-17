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

#include "particleemitter.hpp"
#include "mdlxtranslations.hpp"
#include "mdlxrotations.hpp"
#include "mdlxscalings.hpp"
#include "particleemittervisibilities.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

ParticleEmitter::ParticleEmitter(class ParticleEmitters *particleEmitters) : Node(particleEmitters->mdlx()), GroupMdxBlockMember(particleEmitters, "ParticleEmitter"), m_visibilities(new ParticleEmitterVisibilities(this))
{
}

ParticleEmitter::~ParticleEmitter()
{
	delete this->m_visibilities;
}

std::streamsize ParticleEmitter::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize ParticleEmitter::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "ParticleEmitter", this->name(), 0, true);

	size += Node::writeMdl(ostream);

	if (type() & Type::UnshadedOrEmitterUsesMdl)
		writeMdlProperty(ostream, size, "EmitterUsesMdl");

	if (type() & Type::SortPrimitivesFarZOrEmitterUsesTga)
		writeMdlProperty(ostream, size, "EmitterUsesTga");

	writeMdlStaticValueProperty(ostream, size, "EmissionRate", this->emissionRate());
	writeMdlStaticValueProperty(ostream, size, "Gravity", this->gravity());
	writeMdlStaticValueProperty(ostream, size, "Longitude", this->longitude());
	writeMdlStaticValueProperty(ostream, size, "Latitude", this->latitude());

	size += visibilities()->writeMdl(ostream);

	writeMdlBlock(ostream, size, "Particle");
	writeMdlStaticValueProperty(ostream, size, "LifeSpan", this->lifeSpan());
	writeMdlStaticValueProperty(ostream, size, "InitVelocity", this->initVelocity());
	writeMdlValueProperty(ostream, size, "Path", this->modelPath());
	writeMdlBlockConclusion(ostream, size);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize ParticleEmitter::readMdx(istream &istream) throw (class Exception)
{
	long32 nbytesi;
	std::streamsize size = 0;
	wc3lib::read(istream, nbytesi, size);
	size += Node::readMdx(istream);
	wc3lib::read(istream, this->m_emissionRate, size);
	wc3lib::read(istream, this->m_gravity, size);
	wc3lib::read(istream, this->m_longitude, size);
	wc3lib::read(istream, this->m_latitude, size);
	wc3lib::read(istream, this->m_modelPath, size, modelPathSize);
	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, this->m_lifeSpan, size);
	wc3lib::read(istream, this->m_initVelocity, size);
	size += this->m_visibilities->readMdx(istream);

	return size;
}

std::streamsize ParticleEmitter::writeMdx(ostream &ostream) const throw (class Exception)
{
	// skipping inclusive byte counts of particle emitter
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize size = Node::writeMdx(ostream);
	wc3lib::write(ostream, this->m_emissionRate, size);
	wc3lib::write(ostream, this->m_gravity, size);
	wc3lib::write(ostream, this->m_longitude, size);
	wc3lib::write(ostream, this->m_latitude, size);
	wc3lib::write(ostream, this->m_modelPath, size, modelPathSize);
	wc3lib::write(ostream, this->m_unknown0, size);
	wc3lib::write(ostream, this->m_lifeSpan, size);
	wc3lib::write(ostream, this->m_initVelocity, size);
	size += this->m_visibilities->writeMdx(ostream);

	const long32 nbytesi = boost::numeric_cast<long32>(size);
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
