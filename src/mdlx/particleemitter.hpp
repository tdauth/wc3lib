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

#ifndef WC3LIB_MDLX_PARTICLEEMITTER_HPP
#define WC3LIB_MDLX_PARTICLEEMITTER_HPP

#include <boost/cast.hpp>

#include "node.hpp"
#include "groupmdxblockmember.hpp"
#include "particleemitters.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "ParticleEmitter".
 */
class ParticleEmitter : public Node, public GroupMdxBlockMember
{
	public:
		static const std::size_t modelPathSize = 0x100;

		ParticleEmitter(class ParticleEmitters *particleEmitters);
		virtual ~ParticleEmitter();

		class ParticleEmitters* particleEmitters() const;
		float32 emissionRate() const;
		float32 gravity() const;
		float32 longitude() const;
		float32 latitude() const;
		/**
		 * \return Returns model path with length of \ref modelPathSize.
		 */
		const byte* modelPath() const;
		long32 unknown0() const;
		float32 lifeSpan() const;
		float32 initVelocity() const;
		class ParticleEmitterVisibilities* visibilities() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		//long32 nbytesi;
		//long32 nbytesikg; // inclusive bytecount including KGXXs
		float32 m_emissionRate;
		float32 m_gravity;
		float32 m_longitude;
		float32 m_latitude;
		byte m_modelPath[modelPathSize]; //(0x100 bytes)
		long32 m_unknown0; //(0)
		float32 m_lifeSpan;
		float32 m_initVelocity;
		class ParticleEmitterVisibilities *m_visibilities; //(KPEV)
};

inline class ParticleEmitters* ParticleEmitter::particleEmitters() const
{
	return boost::polymorphic_cast<class ParticleEmitters*>(this->parent());
}

inline float32 ParticleEmitter::emissionRate() const
{
	return this->m_emissionRate;
}

inline float32 ParticleEmitter::gravity() const
{
	return this->m_gravity;
}

inline float32 ParticleEmitter::longitude() const
{
	return this->m_longitude;
}

inline float32 ParticleEmitter::latitude() const
{
	return this->m_latitude;
}

inline const byte* ParticleEmitter::modelPath() const
{
	return this->m_modelPath;
}

inline long32 ParticleEmitter::unknown0() const
{
	return this->m_unknown0;
}

inline float32 ParticleEmitter::lifeSpan() const
{
	return this->m_lifeSpan;
}

inline float32 ParticleEmitter::initVelocity() const
{
	return this->m_initVelocity;
}

inline class ParticleEmitterVisibilities* ParticleEmitter::visibilities() const
{
	return this->m_visibilities;
}

}

}

#endif
