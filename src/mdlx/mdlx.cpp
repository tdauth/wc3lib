#include <iostream> // debug
#include <string>
#include <cstring>

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#include "mdlx.hpp"
#include "version.hpp"
#include "model.hpp"
#include "sequences.hpp"
#include "globalsequences.hpp"
#include "materials.hpp"
#include "textures.hpp"
#include "textureanimations.hpp"
#include "geosets.hpp"
#include "geosetanimations.hpp"
#include "bones.hpp"
#include "lights.hpp"
#include "helpers.hpp"
#include "attachments.hpp"
#include "pivotpoints.hpp"
#include "particleemitters.hpp"
#include "particleemitter2s.hpp"
#include "ribbonemitters.hpp"
#include "cameras.hpp"
#include "events.hpp"
#include "collisionshapes.hpp"

#include "object.hpp"
#include "bone.hpp"
#include "geoset.hpp"
#include "pivotpoint.hpp"

#include "texture.hpp"

#include "../i18n.hpp"

namespace wc3lib
{

namespace mdlx
{

Mdlx::Mdlx() : MdxBlock("MDLX", "", false), m_version(new Version(this)), m_model(new Model(this)), m_sequences(new Sequences(this)), m_globalSequences(new GlobalSequences(this)), m_materials(new Materials(this)), m_textures(new Textures(this)), m_textureAnimations(new TextureAnimations(this)), m_geosets(new Geosets(this)), m_geosetAnimations(new GeosetAnimations(this)), m_bones(new Bones(this)), m_lights(new Lights(this)), m_helpers(new Helpers(this)), m_attachments(new Attachments(this)), m_pivotPoints(new PivotPoints(this)), m_particleEmitters(new ParticleEmitters(this)), m_particleEmitter2s(new ParticleEmitter2s(this)), m_ribbonEmitters(new RibbonEmitters(this)), m_cameras(new Cameras(this)), m_events(new Events(this)), m_collisionShapes(new CollisionShapes(this))
{

}

Mdlx::~Mdlx()
{
	delete this->m_version;
	delete this->m_model;
	delete this->m_sequences;
	delete this->m_globalSequences;
	delete this->m_materials;
	delete this->m_textures;
	delete this->m_textureAnimations;
	delete this->m_geosets;
	delete this->m_geosetAnimations;
	delete this->m_bones;
	delete this->m_lights;
	delete this->m_helpers;
	delete this->m_attachments;
	delete this->m_pivotPoints;
	delete this->m_particleEmitters;
	delete this->m_particleEmitter2s;
	delete this->m_ribbonEmitters;
	delete this->m_cameras;
	delete this->m_events;
	delete this->m_collisionShapes;
}

std::streamsize Mdlx::readMdl(istream &istream) throw (class Exception)
{
	std::streamsize size = this->m_version->readMdl(istream);
	size += this->m_model->readMdl(istream);
	size += this->m_sequences->readMdl(istream);
	size += this->m_globalSequences->readMdl(istream);
	size += this->m_materials->readMdl(istream);
	size += this->m_textures->readMdl(istream);
	size += this->m_textureAnimations->readMdl(istream);
	size += this->m_geosets->readMdl(istream);
	size += this->m_geosetAnimations->readMdl(istream);
	size += this->m_bones->readMdl(istream);
	size += this->m_lights->readMdl(istream);
	size += this->m_helpers->readMdl(istream);
	size += this->m_attachments->readMdl(istream);
	size += this->m_pivotPoints->readMdl(istream);
	size += this->m_particleEmitters->readMdl(istream);
	size += this->m_particleEmitter2s->readMdl(istream);
	size += this->m_ribbonEmitters->readMdl(istream);
	size += this->m_cameras->readMdl(istream);
	size += this->m_events->readMdl(istream);
	size += this->m_collisionShapes->readMdl(istream);

	return size;
}

std::streamsize Mdlx::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = this->m_version->writeMdl(ostream);
	size += this->m_model->writeMdl(ostream);
	size += this->m_sequences->writeMdl(ostream);
	size += this->m_globalSequences->writeMdl(ostream);
	size += this->m_materials->writeMdl(ostream);
	size += this->m_textures->writeMdl(ostream);
	size += this->m_textureAnimations->writeMdl(ostream);
	size += this->m_geosets->writeMdl(ostream);
	size += this->m_geosetAnimations->writeMdl(ostream);
	size += this->m_bones->writeMdl(ostream);
	size += this->m_lights->writeMdl(ostream);
	size += this->m_helpers->writeMdl(ostream);
	size += this->m_attachments->writeMdl(ostream);
	size += this->m_pivotPoints->writeMdl(ostream);
	size += this->m_particleEmitters->writeMdl(ostream);
	size += this->m_particleEmitter2s->writeMdl(ostream);
	size += this->m_ribbonEmitters->writeMdl(ostream);
	size += this->m_cameras->writeMdl(ostream);
	size += this->m_events->writeMdl(ostream);
	size += this->m_collisionShapes->writeMdl(ostream);

	return size;
}

std::streamsize Mdlx::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize bytes = MdxBlock::readMdx(istream);
	bytes += this->m_version->readMdx(istream);
	bytes += this->m_model->readMdx(istream);
	bytes += this->m_sequences->readMdx(istream);
	bytes += this->m_globalSequences->readMdx(istream);
	bytes += this->m_materials->readMdx(istream);
	bytes += this->m_textures->readMdx(istream);
	bytes += this->m_textureAnimations->readMdx(istream);
	bytes += this->m_geosets->readMdx(istream);
	bytes += this->m_geosetAnimations->readMdx(istream);
	bytes += this->m_bones->readMdx(istream);
	bytes += this->m_lights->readMdx(istream);
	bytes += this->m_helpers->readMdx(istream);
	bytes += this->m_attachments->readMdx(istream);
	bytes += this->m_pivotPoints->readMdx(istream);
	bytes += this->m_particleEmitters->readMdx(istream);
	bytes += this->m_particleEmitter2s->readMdx(istream);
	bytes += this->m_ribbonEmitters->readMdx(istream);
	bytes += this->m_cameras->readMdx(istream);
	bytes += this->m_events->readMdx(istream);
	bytes += this->m_collisionShapes->readMdx(istream);

	return bytes;
}

std::streamsize Mdlx::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize bytes = MdxBlock::writeMdx(ostream);
	bytes += this->m_version->writeMdx(ostream);
	bytes += this->m_model->writeMdx(ostream);
	bytes += this->m_sequences->writeMdx(ostream);
	bytes += this->m_globalSequences->writeMdx(ostream);
	bytes += this->m_materials->writeMdx(ostream);
	bytes += this->m_textures->writeMdx(ostream);
	bytes += this->m_textureAnimations->writeMdx(ostream);
	bytes += this->m_geosets->writeMdx(ostream);
	bytes += this->m_geosetAnimations->writeMdx(ostream);
	bytes += this->m_bones->writeMdx(ostream);
	bytes += this->m_lights->writeMdx(ostream);
	bytes += this->m_helpers->writeMdx(ostream);
	bytes += this->m_attachments->writeMdx(ostream);
	bytes += this->m_pivotPoints->writeMdx(ostream);
	bytes += this->m_particleEmitters->writeMdx(ostream);
	bytes += this->m_particleEmitter2s->writeMdx(ostream);
	bytes += this->m_ribbonEmitters->writeMdx(ostream);
	bytes += this->m_cameras->writeMdx(ostream);
	bytes += this->m_events->writeMdx(ostream);
	bytes += this->m_collisionShapes->writeMdx(ostream);

	return bytes;
}

std::size_t Mdlx::replaceTexturePaths(const byte oldTexturePath[0x100], const byte newTexturePath[0x100], std::size_t number)
{
	std::size_t result = 0;

	BOOST_FOREACH(Textures::Members::reference member, this->m_textures->members())
	{
		Texture *texture = boost::polymorphic_cast<Texture*>(&member);

		if (memcmp(texture->texturePath(), oldTexturePath, 0x100) == 0)
		{
			texture->setTexturePath(newTexturePath);
			++result;

			if (number != 0 && result == number)
				break;
		}
	}

	return result;
}

const class Geoset* Mdlx::boneGeoset(const class Bone &bone) const
{
	long32 id = 0;

	BOOST_FOREACH(Geosets::Members::reference geoset, this->m_geosets->members())
	{
		if (id == bone.geosetId())
			return boost::polymorphic_cast<Geoset*>(&geoset);

		++id;
	}

	return 0;
}

const class PivotPoint* Mdlx::nodePivotPoint(const class Node &node) const
{
	long32 id = 0;

	BOOST_FOREACH(Geosets::Members::reference pivotPoint, this->m_pivotPoints->members())
	{
		if (node.id() == id)
			return boost::polymorphic_cast<PivotPoint*>(&pivotPoint);

		++id;
	}

	return 0;
}

const class Node* Mdlx::nodeParent(const class Node &node) const
{
	BOOST_FOREACH(NodePairType nodePair, this->m_nodes)
	{
		if (nodePair.first == node.parentId())
			return nodePair.second;
	}

	return 0;
}

std::list<const class Node*> Mdlx::nodes() const
{
	std::list<const class Node*> result;

	BOOST_FOREACH(NodePairType nodePair, this->m_nodes)
		result.push_back(nodePair.second);

	return result;
}

const class Node* Mdlx::node(long32 id) const
{
	std::map<long32, class Node*>::const_iterator iterator = this->m_nodes.find(id);

	if (iterator == this->m_nodes.end())
		return 0;

	return iterator->second;
}

std::list<const class Node*> Mdlx::children(const class Node &node) const
{
	std::list<const class Node*> result;

	BOOST_FOREACH(NodePairType nodePair, this->m_nodes)
	{
		if (nodePair.second->parentId() == node.id())
			result.push_back(nodePair.second);
	}

	return result;
}

void Mdlx::addNode(long32 id, class Node *node) throw (class Exception)
{
	if (this->m_nodes.find(id) != this->m_nodes.end() && this->m_nodes[id] != node)
		throw Exception(boost::format(_("Mdlx: Node id %1% is already being used by node \"%2%\" and can not be overwritten by node \"%3%\".")) % id % this->m_nodes[id]->name() % node->name());

	this->m_nodes[id] = node;
}

}

}
