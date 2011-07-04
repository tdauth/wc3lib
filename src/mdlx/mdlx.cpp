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

#ifdef BLEND
#include "readblend/abs-file.h"
#include "readblend/readblend.h"
#include "readblend/blendtype.h"
#endif

#include "texture.hpp"

#include "../internationalisation.hpp"

namespace wc3lib
{

namespace mdlx
{

Mdlx::Mdlx() : MdxBlock("MDLX", false), m_version(new Version(this)), m_model(new Model(this)), m_sequences(new Sequences(this)), m_globalSequences(new GlobalSequences(this)), m_materials(new Materials(this)), m_textures(new Textures(this)), m_textureAnimations(new TextureAnimations(this)), m_geosets(new Geosets(this)), m_geosetAnimations(new GeosetAnimations(this)), m_bones(new Bones(this)), m_lights(new Lights(this)), m_helpers(new Helpers(this)), m_attachments(new Attachments(this)), m_pivotPoints(new PivotPoints(this)), m_particleEmitters(new ParticleEmitters(this)), m_particleEmitter2s(new ParticleEmitter2s(this)), m_ribbonEmitters(new RibbonEmitters(this)), m_cameras(new Cameras(this)), m_events(new Events(this)), m_collisionShapes(new CollisionShapes(this))
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

#ifdef BLEND
/**
* The first 12 bytes of every blend-file is the file-header.
* The file-header has information on Blender (version-number) and the PC the blend-file was saved on (pointer-size and endianness).
* This is required as all data inside the blend-file is ordered in that way, because no translation or transformation is done during saving.
*/
struct BlendHeader
{
	char identifier[7]; ///	File identifier (always 'BLENDER')
	char pointerSize; /// Size of a pointer; all pointers in the file are stored in this format. '_' means 4 bytes or 32 bit and '-' means 8 bytes or 64 bits.
	char endianness; /// Type of byte ordering used; 'v' means little endian and 'V' means big endian.
	char versionNumber[3];	/// Version of Blender the file was created in; '248' means version 2.48
};

struct BlendFileBlock
{
	char code[4]; /// Identifier of the file-block
	int size; /// Total length of the data after the file-block-header
	void *oldMemoryAddress; /// Memory address the structure was located when written to disk
	int sdnaIndex; /// Index of the SDNA structure
	int count; /// Number of structure located in this file-block
};
#endif

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

/*
struct BlendReader
{
	std::list<bObj> m_objects;
	std::list<bMesh> m_meshes;
	std::list<bMaterial> m_materials;
	std::list<bTexLayer> m_textureLayers;
};

static BLENDBLOCKCALLBACK_RETURN blendBlockIterator(BLENDBLOCKCALLBACK_ARGS)
{
	const char *tagName = blend_block_get_tagname(blend_file, block);
	const char *typeName = blend_block_get_typename(blend_file, block);
	int entryCount = blend_block_get_entry_count(blend_file, block);
	fprintf(stdout, _("Blend block:\nTag = %s\nType = %s\nEntries = %d\n"), tagName, typeName, entryCount);

	struct BlendReader blendReader = *reinterpret_cast<struct BlendReader*>(userdata);

	for (int i = 0; i < entryCount; ++i)
	{
		BlendObject blendObject = blend_block_get_object(blend_file, block, i);
		const char *objectTypeName = blend_file->types[blendObject.type].name;

		if (strcmp(objectTypeName, "Object") == 0)
		{
			bObj object;
			blend_acquire_obj_from_obj(blend_file, &blendObject, &object, 0);
			blendReader.m_objects.push_back(object);
		}
		else if (strcmp(objectTypeName, "Mesh") == 0)
		{
			bMesh mesh;
			blend_acquire_mesh_from_obj(blend_file, &blendObject, &mesh);
			blendReader.m_meshes.push_back(mesh);
		}
		else if (strcmp(objectTypeName, "TexLayer") == 0)
		{
			bTexLayer textureLayer;
			blend_acquire_texlayer_from_obj(blend_file, &blendObject, &textureLayer);
			blendReader.m_textureLayers.push_back(textureLayer);
		}

		BlendObject dataObject;

		if (blend_object_structure_getfield(blend_file, &dataObject, blendObject, "curscene"))
		{
			if (blend_object_type(blend_file, dataObject) == BLEND_OBJ_POINTER)
			{
				BlendBlock *tmpBlock = 0;

				if (blend_object_getdata(blend_file, &tmpBlock, dataObject))
				{
					BlendObject scene = blend_block_get_object(blend_file, tmpBlock, 0);

					if (blend_object_structure_getfield(blend_file, &dataObject, scene, "id"))
					{
						if (blend_object_type(blend_file, dataObject) == BLEND_OBJ_STRUCT)
						{
							BlendObject dataObject2;

							if (blend_object_structure_getfield(blend_file, &dataObject2, dataObject, "name"))
							{
								char dest[19];
								int max_chars = 20;

								if (blend_object_getstring(blend_file, dataObject2, dest, max_chars))
								{
									printf("dest=%s\n",dest);
								}
							}

						}

					}

				}
			}
			else
			{
			}
		}
	}

	return 1;
}
*/

std::streamsize Mdlx::readBlend(const boost::filesystem::path &filePath) throw (class Exception)
{
	/*
	MY_FILETYPE *file = MY_OPEN_FOR_READ(filePath.c_str());

	if (!file)
	{
		char message[50];
		sprintf(message, _("Was unable to open file \"%s\".\n"), filePath.c_str());

		throw Exception(message);
	}

	BlendFile *blendFile = blend_read(file);

	if (!blendFile)
	{
		MY_CLOSE(file);
		char message[50];
		sprintf(message, _("Was unable to read Blender file \"%s\".\n"), filePath.c_str());

		throw Exception(message);
	}

	// print file information
	blend_dump_typedefs(blendFile);
	blend_dump_blocks(blendFile);

	struct BlendReader blendReader;
	blend_foreach_block(blendFile, blendBlockIterator, &blendReader);

	for (std::list<bObj> ::iterator iterator = blendReader.m_objects.begin(); iterator != blendReader.m_objects.end(); ++iterator)
	{
		fprintf(stdout, _("Got object with name \"%s\".\n"), iterator->name);
	}

	/**

	blend_foreach_block(bf, blockiter, NULL);


	{
		BlendObject obj;
		char want_name[] = "IMmetalrock.jpg";
		if (blend_object_get_by_IDname(bf,&obj, want_name)) {
			fprintf(stderr, "got %s.\n", want_name);
		}
	}


	*/

	/*
	blend_free(blendFile);
	MY_CLOSE(file);
	*/

	return 0;
}

std::streamsize Mdlx::readBlend(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Mdlx::writeBlend(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize Mdlx::read3ds(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Mdlx::write3ds(ostream &ostream) const throw (class Exception)
{
	return 0;
}

/*
void Mdlx::writeMdlGlobalSequences(std::fstream &fstream) throw (Exception)
{
	fstream << "GlobalSequences " << this->m_globalSequencesDurations.size() << " {\n";

	for (std::list<long32>::iterator iterator = this->m_globalSequencesDurations.begin(); iterator != this->m_globalSequencesDurations.end(); ++iterator)
		fstream << "\tDuration " << *iterator << ",\n";

	fstream << "}\n";

	// textures
	fstream << "Textures " << this->m_textures.size() << " {\n";

	for (std::list<struct Texture*>::iterator iterator = this->m_textures.begin(); iterator != this->m_textures.end(); ++iterator)
	{
		fstream
		<< "\tBitmap {\n"
		<< "\t\tImage " << (*iterator)->texturePath << ",\n";

		if ((*iterator)->replaceableId != 0)
			fstream << "\t\tReplaceableId " << (*iterator)->replaceableId << ",\n";

		if ((*iterator)->wrapping == 1 || (*iterator)->wrapping == 3)
			fstream << "\t\tWrapWidth,\n";

		if ((*iterator)->wrapping == 2 || (*iterator)->wrapping == 3)
			fstream << "\t\tWrapHeight,\n";

		fstream << "\t}\n";
	}

	fstream << "}\n";
}

void Mdlx::writeMdlMaterials(std::fstream &fstream) throw (Exception)
{
	fstream << "Materials " << this->m_materials.size() << " {\n";

	for (std::list<struct Material*>::iterator iterator = this->m_materials.begin(); iterator != this->m_materials.end(); ++iterator)
	{
		fstream << "\tMaterial {\n";

		if ((*iterator)->renderMode & 1)
			fstream << "\t\tConstantColor,\n";

		if ((*iterator)->renderMode & 16)
			fstream << "\t\tSortPrimsFarZ,\n";

		if ((*iterator)->renderMode & 32)
			fstream << "\t\tFullResolution,\n";

		if ((*iterator)->priorityPlane != 0)
			fstream << "\t\tPriorityPlane " << (*iterator)->priorityPlane << ",\n";

		bool showCoordId = true;

		for (std::list<struct Layer*>::iterator iterator1 = (*iterator)->layers.begin(); iterator1 != (*iterator)->layers.end(); ++iterator1)
		{
			if ((*iterator1)->coordId == 0)
			{
				showCoordId = false;
				break;
			}
		}

		for (std::list<struct Layer*>::iterator iterator1 = (*iterator)->layers.begin(); iterator1 != (*iterator)->layers.end(); ++iterator1)
		{
			fstream << "\t\tLayer {\n"
			<< "\t\t\tFilterMode ";

			switch ((*iterator1)->filterMode)
			{
				case 0:
					fstream << "None";
					break;

				case 1:
					fstream << "Transparent";
					break;

				case 2:
					fstream << "Blend";
					break;

				case 3:
					fstream << "Additive";
					break;

				case 4:
					fstream << "AddAlpha";
					break;

				case 5:
					fstream << "Modulate";
					break;

				case 6:
					fstream << "Modulate2x";
					break;
			}

			fstream << ",\n";

			if ((*iterator1)->shading & 1)
				fstream << "\t\t\tUnshaded,\n";

			if ((*iterator1)->shading & 2)
				fstream << "\t\t\tSphereEnvMap,\n";

			if ((*iterator1)->shading & 16)
				fstream << "\t\t\tTwoSided,\n";

			if ((*iterator1)->shading & 32)
				fstream << "\t\t\tUnfogged,\n";

			if ((*iterator1)->shading & 64)
				fstream << "\t\t\tNoDepthTest,\n";

			if ((*iterator1)->shading & 128)
				fstream << "\t\t\tNoDepthSet,\n";

			fstream << "\t\t\tstatic (TextureID " << (*iterator1)->textureId << "),\n";

			if ((*iterator1)->tVertexAnimationId != 0xFFFFFFFF)
				fstream << "\t\t\tTVertexAnimId " << (*iterator1)->tVertexAnimationId << ",\n";

			if (showCoordId)
				fstream << "\t\t\tCoordId " << (*iterator1)->coordId << ",\n";

			fstream << "\t\t\tstatic (Alpha " << (*iterator1)->alpha << "),\n"
			<< "\t\t}\n";
		}

		fstream << "\t}\n";
	}

	fstream << "}\n";
}

void Mdlx::writeMdlTextureAnimations(std::fstream &fstream) throw (Exception)
{
	fstream << "TextureAnims " << this->m_textureAnimations.size() << " {\n";

	for (std::list<struct TextureAnimation*>::iterator iterator = this->m_textureAnimations.begin(); iterator != this->m_textureAnimations.end(); ++iterator)
	{
		fstream << "\tTVertexAnim {\n";

		if ((*iterator)->translation != 0)
			fstream << "\t\t(Translation { " << (*iterator)->translation->x << ", " << (*iterator)->translation->y << ", " << (*iterator)->translation->y << " })\n";

		/// @todo InTan and OutTan only appear when Hermite or Bezier. GlobalSeqId only appears when its value is not 0xFFFFFFFF.
		if ((*iterator)->rotation != 0)
			fstream << "\t\t(Rotation { " << (*iterator)->rotation->a << ", " << (*iterator)->rotation->b << ", " << (*iterator)->rotation->c << ", " << (*iterator)->rotation->d << " })\n";

		if ((*iterator)->scaling != 0)
			fstream << "\t\t(Scaling { " << (*iterator)->scaling->x << ", " << (*iterator)->scaling->y << ", " << (*iterator)->scaling->y << " })\n";

		fstream << "\t}\n";
	}

	fstream << "}\n";
}

void Mdlx::writeMdlGeoset(std::fstream &fstream) throw (Exception)
{
	fstream
	<< "Geoset {\n"
	<< "\tVertices " << this->m_geoset->vertices->vertices.size() << " {\n";

	for (std::list<struct Vertice*>::iterator iterator = this->m_geoset->vertices->vertices.begin(); iterator != this->m_geoset->vertices->vertices.end(); ++iterator)
		fstream << "\t\t{ " << (*iterator)->x << ", " << (*iterator)->y << ", " << (*iterator)->z << " },\n";

	fstream << "\t}\n";

	if (this->m_geoset->normals->normals.size() > 0)
	{
		fstream << "\tNormals " << this->m_geoset->normals->normals.size() << " {\n";

		for (std::list<struct Normal*>::iterator iterator = this->m_geoset->normals->normals.begin(); iterator != this->m_geoset->normals->normals.end(); ++iterator)
			fstream << "\t\t{ " << (*iterator)->x << ", " << (*iterator)->y << ", " << (*iterator)->z << " },\n";

		fstream << "\t}\n";
	}

	fstream << "\tTVertices " << this->m_geoset->tvertices->tvertices.size() << " {\n";

	for (std::list<struct Tvertice*>::iterator iterator = this->m_geoset->tvertices->tvertices.begin(); iterator != this->m_geoset->tvertices->tvertices.end(); ++iterator)
		fstream << "\t\t{ " << (*iterator)->x << ", " << (*iterator)->y << "},\n";

	fstream << "\t}\n";

	fstream << "\tVertexGroup {\n";

	for (std::list<byte::iterator iterator = this->m_geoset->vertexGroups->vertexGroups.begin(); iterator != this->m_geoset->vertexGroups->vertexGroups.end(); ++iterator)
		fstream << "\t\t" << *iterator << ",\n";

	fstream << "\t}\n";
	/// @todo Group size this->m_geoset->vertexGroups->vertexGroups.size() as before?!
	fstream << "\tFaces " << this->m_geoset->vertexGroups->vertexGroups.size() << " " << this->m_geoset->primitiveVertices->triangles.size() << " {\n"
	<< "\t\tTriangles {\n"
	<< "\t\t\t{ "

	for (std::list<short32>::iterator iterator = this->m_geoset->primitiveVertices->triangles.begin(); iterator != this->m_geoset->primitiveVertices->triangles.end(); ++iterator)
		fstream << *iterator << ", ";

	fstream
	<< "\t\t\t},\n"
	<< "\t\t}\n"
	<< "\t}\n";

	fstream
	<< "\tGroups " << this->m_geoset->matriceGroupCounts->groupCounts.size() << " " << this->m_geoset->matrices.size() << " {\n"
	<< "\t\tMatrices { ";

	for (std::list<long32>::iterator iterator = this->m_geoset->matrices.begin(); iterator != this->m_geoset->matrices.end(); ++iterator)
		fstream << *iterator << ", ";

	fstream
	<< "},\n"
	<< "\t}\n";

	if (this->m_geoset->m_minExtX != 0.0 || this->m_geoset->m_minExtY != 0.0 || this->m_geoset->m_minExtZ != 0.0)
		fstream << "\tMinimumExtent { " << this->m_geoset->m_minExtX << ", " << this->m_geoset->m_minExtY << ", " << this->m_geoset->m_minExtZ << " },\n";

	if (this->m_geoset->m_maxExtX != 0.0 || this->m_geoset->m_maxExtY != 0.0 || this->m_geoset->m_maxExtZ != 0.0)
		fstream << "\tMaximumExtent { " << this->m_geoset->m_minExtX << ", " << this->m_geoset->m_minExtY << ", " << this->m_geoset->m_minExtZ << " },\n";

	if (this->m_geoset->boundsRadius != 0.0)
		fstream << "\tBoundsRadius " << this->m_geoset->boundsRadius << ",\n";

	for (std::list<struct GAnimation*>::iterator iterator = this->m_geoset->ganimations.begin(); iterator != this->m_geoset->ganimations.end(); ++iterator)
	{
		fstream << "\tAnim {\n";

		if ((*iterator)->minExtX != 0.0 || (*iterator)->minExtY != 0.0 || (*iterator)->minExtZ != 0.0)
			fstream << "\t\tMinimumExtent { " << (*iterator)->minExtX << ", " << (*iterator)->minExtY << ", " << (*iterator)->minExtZ << " },\n";

		if ((*iterator)->maxExtX != 0.0 || (*iterator)->maxExtY != 0.0 || (*iterator)->maxExtZ != 0.0)
			fstream << "\t\tMaximumExtent { " << (*iterator)->maxExtX << ", " << (*iterator)->maxExtY << ", " << (*iterator)->maxExtZ << " },\n";

		if ((*iterator)->boundsRadius != 0.0)
			fstream << "\t\tBoundsRadius " << (*iterator)->boundsRadius << ",\n";

		fstream << "\t}\n";
	}

	fstream
	<< "\tMaterialID " << this->m_geoset->materialId << ",\n"
	<< "\tSelectionGroup " << this->m_geoset->selectionGroup << ",\n";

	if (this->m_geoset->selectable == 4)
		fstream << "\tUnselectable,\n";

	fstream << "}\n";
}

void Mdlx::writeMdlGeosetAnimations(std::fstream &fstream) throw (Exception)
{
	for (std::list<struct GeosetAnimation*>::iterator iterator = this->m_geosetAnimations->geosetAnimations.begin(); iterator != this->m_geosetAnimations->geosetAnimations.end(); ++iterator)
	{
		fstream << "GeosetAnim {\n";

		if ((*iterator)->colorAnimation == 1 || (*iterator)->colorAnimation == 3)
			fstream << "\tDropShadow,\n";

		fstream << "\tstatic (Alpha " << (*iterator)->staticAlpha << "),\n";

		/// @todo This flag?
		if ((*iterator)->colorAnimation > 1)
			fstream << "\tstatic (Color { " << (*iterator)->colorBlue << ", " << (*iterator)->colorGreen << ", " << (*iterator)->colorRed << " }),\n";

		fstream
		<< "\tGeosetId " << (*iterator)->geosetId << ",\n"
		<< "}\n";
	}
}

void Mdlx::writeMdlBones(std::fstream &fstream) throw (Exception)
{
	for (std::list<struct Bone*>::iterator iterator = this->m_bones->bones.begin(); iterator != this->m_bones->bones.end(); ++iterator)
	{
		fstream
		<< "Bone " << (*iterator)->object->name << " {\n"
		<< "\tObjectId " << (*iterator)->object->objectId << ",\n"
		<< "\tParent " << (*iterator)->object->parent << ",\n"
		;
		/* << "\tBillboardedLockZ,
		BillboardedLockY,
		BillboardedLockX,
		Billboarded,
		CameraAnchored,
		DontInherit { <Rotation|Translation|Scaling> },
		*/
/*
		if ((*iterator)->geosetId == -1)
			fstream << "\tGeosetId Multiple,\n";
		else
			fstream << "\tGeosetId " << (*iterator)->geosetId << ",\n";

		if ((*iterator)->geosetAnimationId == -1)
			fstream << "\tGeosetAnimId None,\n";
		else
			fstream << "\tGeosetAnimId " << (*iterator)->geosetAnimationId << ",\n";

		/*
		(Translation { <float_x>, <float_y>, <float_z> })
		(Rotation { <float_a>, <float_b>, <float_c>, <float_d> })
		(Scaling { <float_x>, <float_y>, <float_z> })
		(Visibility <float>)
		*//*
	}
}

void Mdlx::writeMdlHelpers(std::fstream &fstream) throw (Exception)
{
	//helpers
	// Observe properties of an Object.
	for (std::list<struct Object*>::iterator iterator = this->m_helpers->helpers.begin(); iterator != this->m_helpers->helpers.end(); ++iterator)
	{
		fstream
		<< "Helper " << (*iterator)->name << " {\n"
		<< "\tObjectId " << (*iterator)->objectId << ",\n"
		<< "\tParent " << (*iterator)->parent << ",\n";
		/*
		<< BillboardedLockZ,
		BillboardedLockY,
		BillboardedLockX,
		Billboarded,
		CameraAnchored,
		DontInherit { <Rotation|Translation|Scaling> },
		(Translation { <float_x>, <float_y>, <float_z> })
		(Rotation { <float_a>, <float_b>, <float_c>, <float_d> })
		(Scaling { <float_x>, <float_y>, <float_z> })
		(Visibility <float>)
		*/
		/*
	}
}
*/

std::size_t Mdlx::replaceTexturePaths(const ascii oldTexturePath[0x100], const ascii newTexturePath[0x100], std::size_t number)
{
	std::size_t result = 0;

	BOOST_FOREACH(class Texture *texture, this->m_textures->textures())
	{
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

	BOOST_FOREACH(const class Geoset *geoset, this->m_geosets->geosets())
	{
		if (id == bone.geosetId())
			return geoset;

		++id;
	}

	return 0;
}

const class PivotPoint* Mdlx::nodePivotPoint(const class Node &node) const
{
	long32 id = 0;

	BOOST_FOREACH(const class PivotPoint *pivotPoint, this->m_pivotPoints->pivotPoints())
	{
		if (node.id() == id)
			return pivotPoint;

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
