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

class Version;
class Model;
class Sequences;
class GlobalSequences;
class Materials;
class Textures;
class TextureAnimations;
class Geosets;
class GeosetAnimations;
class Bones;
class Lights;
class Helpers;
class Attachments;
class PivotPoints;
class ParticleEmitters;
class ParticleEmitter2s;
class RibbonEmitters;
class Cameras;
class Events;
class CollisionShapes;

/**
 * \brief One single model file from the MDX or the MDL format.
 *
 * This class provides an abstract interface to one single model file of the MDX or the MDL format.
 * Since both formats cover the same specification in a different way one single interface is enough
 * to provide access to all attributes like nodes, geosets, animations etc.
 *
 * For further information about both formats see \ref mdlxsection or \ref mdl and \ref mdx.
 *
 * As MDLX is a node-based format you can access various properties by their node ids (\ref long32 integers).
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

		Version* modelVersion() const;
		Model* model() const;
		Sequences* sequences() const;
		GlobalSequences* globalSequences() const;
		Materials* materials() const;
		Textures* textures() const;
		TextureAnimations* textureAnimations() const;
		Geosets* geosets() const;
		GeosetAnimations* geosetAnimations() const;
		Bones* bones() const;
		Lights* lights() const;
		Helpers* helpers() const;
		Attachments* attachments() const;
		PivotPoints* pivotPoints() const;
		ParticleEmitters* particleEmitters() const;
		ParticleEmitter2s* particleEmitter2s() const;
		RibbonEmitters* ribbonEmitters() const;
		Cameras* cameras() const;
		Events* events() const;
		CollisionShapes* collisionShapes() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		Version *m_version; //VERS
		Model *m_model; //MODL
		Sequences *m_sequences; //SEQS
		GlobalSequences *m_globalSequences; //GLBS
		Materials *m_materials; //MTLS
		Textures *m_textures; //TEXS
		TextureAnimations *m_textureAnimations; //TXAN
		Geosets *m_geosets;
		GeosetAnimations *m_geosetAnimations;
		Bones *m_bones;
		Lights *m_lights; //LITE
		Helpers *m_helpers;
		Attachments *m_attachments;
		PivotPoints *m_pivotPoints; //PIVT
		ParticleEmitters *m_particleEmitters; //PREM
		ParticleEmitter2s *m_particleEmitter2s; //PRE2
		RibbonEmitters *m_ribbonEmitters; //RIBB
		Cameras *m_cameras;
		Events *m_events;
		CollisionShapes *m_collisionShapes;
};

inline Version* Mdlx::modelVersion() const
{
	return this->m_version;
}

inline Model* Mdlx::model() const
{
	return this->m_model;
}

inline Sequences* Mdlx::sequences() const
{
	return this->m_sequences;
}

inline GlobalSequences* Mdlx::globalSequences() const
{
	return this->m_globalSequences;
}

inline Materials* Mdlx::materials() const
{
	return this->m_materials;
}

inline Textures* Mdlx::textures() const
{
	return this->m_textures;
}

inline TextureAnimations* Mdlx::textureAnimations() const
{
	return this->m_textureAnimations;
}

inline Geosets* Mdlx::geosets() const
{
	return this->m_geosets;
}

inline GeosetAnimations* Mdlx::geosetAnimations() const
{
	return this->m_geosetAnimations;
}

inline Bones* Mdlx::bones() const
{
	return this->m_bones;
}

inline Lights* Mdlx::lights() const
{
	return this->m_lights;
}

inline Helpers* Mdlx::helpers() const
{
	return this->m_helpers;
}

inline Attachments* Mdlx::attachments() const
{
	return this->m_attachments;
}

inline PivotPoints* Mdlx::pivotPoints() const
{
	return this->m_pivotPoints;
}

inline ParticleEmitters* Mdlx::particleEmitters() const
{
	return this->m_particleEmitters;
}

inline ParticleEmitter2s* Mdlx::particleEmitter2s() const
{
	return this->m_particleEmitter2s;
}

inline RibbonEmitters* Mdlx::ribbonEmitters() const
{
	return this->m_ribbonEmitters;
}

inline Cameras* Mdlx::cameras() const
{
	return this->m_cameras;
}

inline Events* Mdlx::events() const
{
	return this->m_events;
}

inline CollisionShapes* Mdlx::collisionShapes() const
{
	return this->m_collisionShapes;
}

}

}

#endif
