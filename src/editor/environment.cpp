
#include "environment.hpp"
#include "../map/shadow.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

Ogre::TerrainGroup* Environment::convert(Ogre::sceneManager *sceneManager, const wc3lib::map::Environment& environment, const QString &fileName, const QString &fileExtension)
{
	environment.mapWidth()
	/*
	 * Each tilepoint is divided into 4x4 shadow points.
	 */
	Ogre::TerrainGroup *result = new Ogre::TerrainGroup(sceneManager, Ogre::Terrain::ALIGN_X_Z, 4, 12000.0f);
	result->setFilenameConvention(Ogre::String(fileName.toStdString()), Ogre::String(fileExtension.toStdString()));
	result->setOrigin(Ogre::Vector3::ZERO);

	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = result->getDefaultImportSettings();
	defaultimp.terrainSize = 4;
	defaultimp.worldSize = 12000.0f;
	defaultimp.inputScale = 600; // due terrain.png is 8 bpp
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;

	// texture layers
	defaultimp.layerList.resize(environment.groundTilesetsIds().size());

	// TODO get terrain data
	defaultimp.layerList[0].worldSize = 100; // decides how big each splat of textures is going to be. A smaller value will increase the resolution of the rendered texture layer.
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

	for (int32 x = 0; x < environment.mapWidth(); ++x)
	{
		for (int32 y = 0; y < environment.mapHeight(); ++y)
		{
			//result->defineTerrain(x, y, &img);
			result->defineTerrain(x, y);
		}
	}

	result->loadAllTerrains(true);

	/*
	 * Iterate each tilepoint.
	 */
	for (int32 x = 0; x < environment.mapWidth(); ++x)
	{
		for (int32 y = 0; y < environment.mapHeight(); ++y)
		{
			const map::Tilepoint *tilepoint = environment.tilepoints()[x][y];
			Ogre::Terrain* t = result->getTerrain(x, y);
			t->setHeightAtPoint(0, 0, tilepoint->groundHeight());
			//initBlendMaps(t);
			Ogre::TerrainLayerBlendMap *blendMap0 = t->getLayerBlendMap(tilepoint->groundTextureType());
		}
        }

	// mTerrainGroup->freeTemporaryResources();
}

Ogre::ManualObject* Environment::convert(Ogre::SceneManager* sceneManager, const map::Environment& environment, const Ogre::String &name)
{
	Ogre::ManualObject *object = sceneManager->createManualObject(name);
	object->estimateVertexCount(environment.tilepoints().size());
	object->estimateIndexCount(environment.tilepoints().size());
	object->begin("MaterialName", Ogre::RenderOperation::OT_POINT_LIST);
	// build vertices
	Ogre::uint32 index = 0;
	//qDebug() << "TVertices " << geoset.textureVertices()->textureVertices().size() << "{";
	//qDebug() << "Normals " << geoset.normals()->normals().size() << "(";
	//qDebug() << "Vertices " << geoset.vertices()->vertices().size() << "(";

	for (int32 x = 0; x < environment.mapWidth(); ++x)
	{
		for (int32 y = 0; y < environment.mapHeight(); ++y)
		{
			const map::Tilepoint *tilepoint = environment.tilepoints()[x][y];

			object->position(Ogre::Vector3(x, y, tilepoint->groundHeight()));
			//object->normal(ogreVector3(normal->vertexData()));
			//object->textureCoord(ogreVector2(textureVertex->vertexData()));
		}
	}

	/*
	 * Index: x y
	 * 
	 * TODO http://www.ogre3d.org/tikiwiki/ManualObject
	 * set material with textures???
	 */
	for (int32 x = 0; x < environment.mapWidth(); ++x)
	{
		for (int32 y = 0; y < environment.mapHeight(); ++y)
		{
			// TODO 2 dimensional index, build quad of tilepoints.
			//object->quad(x * y   );
		}
	}


	object->end();

	return object;
}

}

}
