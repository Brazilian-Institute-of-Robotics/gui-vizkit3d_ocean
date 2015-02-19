/*
 * Sce‘neModel.cpp
 *
 *  Created on: Nov 25, 2014
 *      Author: gustavo
 */

#include <osg/LightSource>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgDB/FileUtils>
#include <stdexcept>

#include <osgOcean/ShaderManager>
#include "SceneModel.h"

osg::Vec4f intColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255)
{
	float div = 1.f/255.f;
	return osg::Vec4f( div*(float)r, div*(float)g, div*float(b), div*(float)a );
}

SceneModel::SceneModel(const osg::Vec2f& windDirection,
					float windSpeed,
					float depth,
					float reflectionDamping,
					float scale,
					bool  isChoppy,
					float choppyFactor,
					float crestFoamHeight) : _sceneType(CLEAR)
{
    _cubemapDirs.push_back( "vizkit3d_ocean/textures/sky_clear" );
    _cubemapDirs.push_back( "vizkit3d_ocean/textures/sky_dusk" );
    _cubemapDirs.push_back( "vizkit3d_ocean/textures/sky_fair_cloudy" );

    _lightColors.push_back( intColor( 105,138,174 ));
    _lightColors.push_back( intColor( 105,138,174 ));
    _lightColors.push_back( intColor( 105,138,174 ));

	_fogColors.push_back( intColor( 199,226,255 ) );
	_fogColors.push_back( intColor( 244,228,179 ) );
	_fogColors.push_back( intColor( 172,224,251 ) );

    _sunPositions.push_back( osg::Vec3f(326.573, 1212.99 ,1275.19) );
    _sunPositions.push_back( osg::Vec3f(520.f, 1900.f, 550.f ) );
    _sunPositions.push_back( osg::Vec3f(-1056.89f, -771.886f, 1221.18f ) );

    _sunDiffuse.push_back( intColor( 191, 191, 191 ) );
    _sunDiffuse.push_back( intColor( 251, 251, 161 ) );
    _sunDiffuse.push_back( intColor( 191, 191, 191 ) );

    _waterFogColors.push_back( intColor(27,57,109) );
    _waterFogColors.push_back( intColor(44,69,106 ) );
    _waterFogColors.push_back( intColor(84,135,172 ) );

    _underwaterAttenuations.push_back( osg::Vec3f(0.015f, 0.0075f, 0.005f) );
    _underwaterAttenuations.push_back( osg::Vec3f(0.015f, 0.0075f, 0.005f) );
    _underwaterAttenuations.push_back( osg::Vec3f(0.008f, 0.003f, 0.002f) );

    _underwaterDiffuse.push_back( intColor(27,57,109) );
    _underwaterDiffuse.push_back( intColor(44,69,106) );
    _underwaterDiffuse.push_back( intColor(84,135,172) );

    build(windDirection, windSpeed, depth, reflectionDamping, scale, isChoppy, choppyFactor, crestFoamHeight);
}

SceneModel::~SceneModel()
{
}

void SceneModel::build(const osg::Vec2f& windDirection,
					float windSpeed,
					float depth,
					float reflectionDamping,
					float waveScale,
					bool  isChoppy,
					float choppyFactor,
					float crestFoamHeight)
{
    _scene = new osg::Group();

    _cubemap = loadCubeMapTextures( _cubemapDirs[_sceneType] );


    osg::LightSource* lightSource = new osg::LightSource();
    lightSource->setLocalStateSetModes();

    _light = lightSource->getLight();
    _light->setLightNum(0);
    _light->setAmbient( osg::Vec4d(0.3f, 0.3f, 0.3f, 1.0f ));
    _light->setDiffuse( _sunDiffuse[_sceneType] );
    _light->setSpecular(osg::Vec4d( 0.1f, 0.1f, 0.1f, 1.0f ) );
    _light->setPosition( osg::Vec4f(_sunPositions[_sceneType], 1.f) ); // point light

    _scene->addChild( lightSource );
    _scene->addChild( _oceanScene.get() );
}

SceneModel* SceneModel::create()
{
	SceneModel *instance = new SceneModel();
	osgOcean::ShaderManager::instance().enableShaders(true);
	return instance;
}

osg::Group* SceneModel::getScene()
{
	return _scene.get();
}

osg::ref_ptr<osg::TextureCubeMap> SceneModel::loadCubeMapTextures(const std::string& dir)
{
	enum {POS_X, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z};

	std::string filenames[6];

	filenames[POS_X] = osgDB::findDataFile(dir + "/east.png");
	filenames[NEG_X] = osgDB::findDataFile(dir + "/west.png");
	filenames[POS_Z] = osgDB::findDataFile(dir + "/north.png");
	filenames[NEG_Z] = osgDB::findDataFile(dir + "/south.png");
	filenames[POS_Y] = osgDB::findDataFile(dir + "/down.png");
	filenames[NEG_Y] = osgDB::findDataFile(dir + "/up.png");

	for (int i = 0; i < 6; ++i)
	{
		if (filenames[i].empty())
			throw std::runtime_error("cannot find required cubemap texture in " + dir);
	}

	osg::ref_ptr<osg::TextureCubeMap> cubeMap = new osg::TextureCubeMap;
	cubeMap->setInternalFormat(GL_RGBA);

	cubeMap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	cubeMap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

	cubeMap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	cubeMap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_X,  );
	cubeMap->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile( filenames[POS_X] ) );
	cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile( filenames[NEG_Y] ) );
	cubeMap->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile( filenames[POS_Y] ) );
	cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile( filenames[NEG_Z] ) );
	cubeMap->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile( filenames[POS_Z] ) );

	return cubeMap;
}
