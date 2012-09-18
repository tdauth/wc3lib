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

#ifndef WC3LIB_MDLX_MDLX_HPP
#define WC3LIB_MDLX_MDLX_HPP

#include "mdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * Provides access to one single MDX/MDL model which can be read from any input stream.
 * Since MDLX is a node-based format you can access various properties by their node ids (\ref long32 integers).
 * Use \ref Mdlx::nodePivotPoint to get a node's pivot point which is required for its correct position.
 * Nodes are stored in a map (\ref std::map) which allows you to access them as fast as possible since default maps are sorted.
 * \note All properties are allocated on object's construction (not only if required during reading process).
 *
 * long/float size: 32-bit
 * \todo OBJ == ?!
 * \todo KATV == float?!
 */
class Mdlx : public MdxBlock
{
	public:
		Mdlx();
		virtual ~Mdlx();

		class Version* vers() const;
		class Model* model() const;
		class Sequences* sequences() const;
		class GlobalSequences* globalSequences() const;
		class Materials* materials() const;
		class Textures* textures() const;
		class TextureAnimations* textureAnimations() const;
		class Geosets* geosets() const;
		class GeosetAnimations* geosetAnimations() const;
		class Bones* bones() const;
		class Lights* lights() const;
		class Helpers* helpers() const;
		class Attachments* attachments() const;
		class PivotPoints* pivotPoints() const;
		class ParticleEmitters* particleEmitters() const;
		class ParticleEmitter2s* particleEmitter2s() const;
		class RibbonEmitters* ribbonEmitters() const;
		class Cameras* cameras() const;
		class Events* events() const;
		class CollisionShapes* collisionShapes() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		class Version *m_version; //VERS
		class Model *m_model; //MODL
		class Sequences *m_sequences; //SEQS
		class GlobalSequences *m_globalSequences; //GLBS
		class Materials *m_materials; //MTLS
		class Textures *m_textures; //TEXS
		class TextureAnimations *m_textureAnimations; //TXAN
		class Geosets *m_geosets;
		class GeosetAnimations *m_geosetAnimations;
		class Bones *m_bones;
		class Lights *m_lights; //LITE
		class Helpers *m_helpers;
		class Attachments *m_attachments;
		class PivotPoints *m_pivotPoints; //PIVT
		class ParticleEmitters *m_particleEmitters; //PREM
		class ParticleEmitter2s *m_particleEmitter2s; //PRE2
		class RibbonEmitters *m_ribbonEmitters; //RIBB
		class Cameras *m_cameras;
		class Events *m_events;
		class CollisionShapes *m_collisionShapes;
};

inline class Version* Mdlx::vers() const
{
	return this->m_version;
}

inline class Model* Mdlx::model() const
{
	return this->m_model;
}

inline class Sequences* Mdlx::sequences() const
{
	return this->m_sequences;
}

inline class GlobalSequences* Mdlx::globalSequences() const
{
	return this->m_globalSequences;
}

inline class Materials* Mdlx::materials() const
{
	return this->m_materials;
}

inline class Textures* Mdlx::textures() const
{
	return this->m_textures;
}

inline class TextureAnimations* Mdlx::textureAnimations() const
{
	return this->m_textureAnimations;
}

inline class Geosets* Mdlx::geosets() const
{
	return this->m_geosets;
}

inline class GeosetAnimations* Mdlx::geosetAnimations() const
{
	return this->m_geosetAnimations;
}

inline class Bones* Mdlx::bones() const
{
	return this->m_bones;
}

inline class Lights* Mdlx::lights() const
{
	return this->m_lights;
}

inline class Helpers* Mdlx::helpers() const
{
	return this->m_helpers;
}

inline class Attachments* Mdlx::attachments() const
{
	return this->m_attachments;
}

inline class PivotPoints* Mdlx::pivotPoints() const
{
	return this->m_pivotPoints;
}

inline class ParticleEmitters* Mdlx::particleEmitters() const
{
	return this->m_particleEmitters;
}

inline class ParticleEmitter2s* Mdlx::particleEmitter2s() const
{
	return this->m_particleEmitter2s;
}

inline class RibbonEmitters* Mdlx::ribbonEmitters() const
{
	return this->m_ribbonEmitters;
}

inline class Cameras* Mdlx::cameras() const
{
	return this->m_cameras;
}

inline class Events* Mdlx::events() const
{
	return this->m_events;
}

inline class CollisionShapes* Mdlx::collisionShapes() const
{
	return this->m_collisionShapes;
}

}

}

#endif
