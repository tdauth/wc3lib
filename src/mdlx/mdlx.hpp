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

#include <vector>

#include "platform.hpp"
#include "model.hpp"
#include "sequence.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "soundtrack.hpp"
#include "textureanimation.hpp"
#include "geoset.hpp"
#include "geosetanimation.hpp"
#include "bone.hpp"
#include "light.hpp"
#include "attachment.hpp"
#include "particleemitter.hpp"
#include "particleemitter2.hpp"
#include "ribbonemitter.hpp"
#include "camera.hpp"
#include "event.hpp"
#include "collisionshape.hpp"

namespace wc3lib
{

namespace mdlx
{

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
 *
 * long/float size: 32-bit
 * \todo OBJ == ?!
 * \todo KATV == float?!
 */
class Mdlx : public Format
{
	public:
		typedef std::vector<Sequence> Sequences;
		typedef std::vector<long32> GlobalSequences;
		typedef std::vector<Material> Materials;
		typedef std::vector<Texture> Textures;
		typedef std::vector<SoundTrack> SoundTracks;
		typedef std::vector<TextureAnimation> TextureAnimations;
		typedef std::vector<Geoset> Geosets;
		typedef std::vector<GeosetAnimation> GeosetAnimations;
		typedef std::vector<Bone> Bones;
		typedef std::vector<Light> Lights;
		typedef std::vector<Object> Helpers;
		typedef std::vector<Attachment> Attachments;
		typedef std::vector<VertexData> PivotPoints;
		typedef std::vector<ParticleEmitter> ParticleEmitters;
		typedef std::vector<ParticleEmitter2> ParticleEmitter2s;
		typedef std::vector<RibbonEmitter> RibbonEmitters;
		typedef std::vector<Camera> Cameras;
		typedef std::vector<Event> Events;
		typedef std::vector<CollisionShape> CollisionShapes;

		static const long32 tftVersion;
		static const long32 reforgedVersion;
		static const long32 currentVersion;

		Mdlx();
		virtual ~Mdlx();

		void setModelVersion(long32 version);
		long32 modelVersion() const;
		void setModel(const Model &model);
		const Model& model() const;
		void setSequence(const Sequences &sequences);
		const Sequences& sequences() const;
		void setGlobalSequences(const GlobalSequences &globalSequences);
		const GlobalSequences& globalSequences() const;
		void setTextures(const Textures &textures);
		const Textures& textures() const;
		void setSoundTracks(const SoundTracks &soundTracks);
		const SoundTracks& soundTracks() const;
		void setMaterials(const Materials &materials);
		const Materials& materials() const;
		void setTextureAnimations(const TextureAnimations &textureAnimations);
		const TextureAnimations& textureAnimations() const;
		void setGeosets(const Geosets &geosets);
		const Geosets& geosets() const;
		void setGeosetAnimations(const GeosetAnimations &geosetAnimations);
		const GeosetAnimations& geosetAnimations() const;
		void setBones(const Bones &bones);
		const Bones& bones() const;
		void setLights(const Lights &lights);
		const Lights& lights() const;
		const Helpers& helpers() const;
		const Attachments& attachments() const;
		const PivotPoints& pivotPoints() const;
		const ParticleEmitters& particleEmitters() const;
		const ParticleEmitter2s& particleEmitter2s() const;
		const RibbonEmitters& ribbonEmitters() const;
		const Cameras& cameras() const;
		const Events& events() const;
		const CollisionShapes& collisionShapes() const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		long32 m_modelVersion;
		Model m_model;
		Sequences m_sequences;
		GlobalSequences m_globalSequences;
		Textures m_textures;
		SoundTracks m_soundTracks;
		Materials m_materials;
		TextureAnimations m_textureAnimations;
		Geosets m_geosets;
		GeosetAnimations m_geosetAnimations;
		Bones m_bones;
		Lights m_lights;
		Helpers m_helpers;
		Attachments m_attachments;
		PivotPoints m_pivotPoints;
		ParticleEmitters m_particleEmitters;
		ParticleEmitter2s m_particleEmitter2s;
		RibbonEmitters m_ribbonEmitters;
		Cameras m_cameras;
		Events m_events;
		CollisionShapes m_collisionShapes;
};

inline void Mdlx::setModelVersion(long32 version)
{
	this->m_modelVersion = version;
}

inline long32 Mdlx::modelVersion() const
{
	return this->m_modelVersion;
}

inline void Mdlx::setModel(const Model &model)
{
	this->m_model = model;
}

inline const Model& Mdlx::model() const
{
	return this->m_model;
}

inline void Mdlx::setSequence(const Mdlx::Sequences &sequences)
{
	this->m_sequences = sequences;
}

inline const Mdlx::Sequences& Mdlx::sequences() const
{
	return this->m_sequences;
}

inline void Mdlx::setGlobalSequences(const Mdlx::GlobalSequences &globalSequences)
{
	this->m_globalSequences = globalSequences;
}

inline const Mdlx::GlobalSequences& Mdlx::globalSequences() const
{
	return this->m_globalSequences;
}

inline void Mdlx::setTextures(const Mdlx::Textures &textures)
{
	this->m_textures = textures;
}

inline const Mdlx::Textures& Mdlx::textures() const
{
	return this->m_textures;
}

inline void Mdlx::setSoundTracks(const Mdlx::SoundTracks &soundTracks)
{
	this->m_soundTracks = soundTracks;
}

inline const Mdlx::SoundTracks& Mdlx::soundTracks() const
{
	return this->m_soundTracks;
}

inline void Mdlx::setMaterials(const Mdlx::Materials &materials)
{
	this->m_materials = materials;
}

inline const Mdlx::Materials& Mdlx::materials() const
{
	return this->m_materials;
}

inline void Mdlx::setTextureAnimations(const Mdlx::TextureAnimations &textureAnimations)
{
	this->m_textureAnimations = textureAnimations;
}

inline const Mdlx::TextureAnimations& Mdlx::textureAnimations() const
{
	return this->m_textureAnimations;
}

inline void Mdlx::setGeosets(const Geosets &geosets)
{
	this->m_geosets = geosets;
}

inline const Mdlx::Geosets& Mdlx::geosets() const
{
	return this->m_geosets;
}

inline void Mdlx::setGeosetAnimations(const Mdlx::GeosetAnimations& geosetAnimations)
{
	this->m_geosetAnimations = geosetAnimations;
}

inline const Mdlx::GeosetAnimations& Mdlx::geosetAnimations() const
{
	return this->m_geosetAnimations;
}

inline void Mdlx::setBones(const Mdlx::Bones &bones)
{
	this->m_bones = bones;
}

inline const Mdlx::Bones& Mdlx::bones() const
{
	return this->m_bones;
}

inline void Mdlx::setLights(const Mdlx::Lights &lights)
{
	this->m_lights = lights;
}

inline const Mdlx::Lights& Mdlx::lights() const
{
	return this->m_lights;
}

inline const Mdlx::Helpers& Mdlx::helpers() const
{
	return this->m_helpers;
}

inline const Mdlx::Attachments& Mdlx::attachments() const
{
	return this->m_attachments;
}

inline const Mdlx::PivotPoints& Mdlx::pivotPoints() const
{
	return this->m_pivotPoints;
}

inline const Mdlx::ParticleEmitters& Mdlx::particleEmitters() const
{
	return this->m_particleEmitters;
}

inline const Mdlx::ParticleEmitter2s& Mdlx::particleEmitter2s() const
{
	return this->m_particleEmitter2s;
}

inline const Mdlx::RibbonEmitters& Mdlx::ribbonEmitters() const
{
	return this->m_ribbonEmitters;
}

inline const Mdlx::Cameras& Mdlx::cameras() const
{
	return this->m_cameras;
}

inline const Mdlx::Events& Mdlx::events() const
{
	return this->m_events;
}

inline const Mdlx::CollisionShapes& Mdlx::collisionShapes() const
{
	return this->m_collisionShapes;
}

}

}

#endif
