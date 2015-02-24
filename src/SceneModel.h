/*
 * SceneModel.h
 *
 *  Created on: Nov 25, 2014
 *      Author: gustavo
 */

#ifndef SCENEMODEL_H_
#define SCENEMODEL_H_

#include <osg/Vec2f>
#include <osg/Group>
#include <osg/Switch>
#include <osgOcean/OceanScene>
#include <osgOcean/FFTOceanSurface>

#include "SkyDome.hpp"

class SceneModel {

public:

	static SceneModel* create();

	osg::Group* getScene();

private:

	enum SCENE_TYPE
	{
		CLEAR,
		DUSK,
		CLOUDY
	};

	SceneModel(const osg::Vec2f& windDirection = osg::Vec2f(1.0f,1.0f),
			float windSpeed = 12.f,
			float depth = 1000.0f,
			float reflectionDamping = 0.0f,
			float scale = 1e-8,
			bool  isChoppy = true,
			float choppyFactor = -2.5f,
			float crestFoamHeight = 2.0f);

	virtual ~SceneModel();

	void build(const osg::Vec2f& windDirection,
			float windSpeed,
			float depth,
			float reflectionDamping,
			float waveScale,
			bool  isChoppy,
			float choppyFactor,
			float crestFoamHeight);

	osg::ref_ptr<osg::TextureCubeMap> loadCubeMapTextures(const std::string& dir);

    osg::ref_ptr<osgOcean::OceanScene> _oceanScene;
    osg::ref_ptr<osgOcean::FFTOceanSurface> _oceanSurface;
    osg::ref_ptr<osg::TextureCubeMap> _cubemap;

    std::vector<std::string> _cubemapDirs;
    std::vector<osg::Vec4f>  _lightColors;
    std::vector<osg::Vec4f>  _fogColors;
    std::vector<osg::Vec4f>  _waterFogColors;
    std::vector<osg::Vec3f>  _underwaterAttenuations;
    std::vector<osg::Vec4f>  _underwaterDiffuse;
    std::vector<osg::Vec3f>  _sunPositions;
    std::vector<osg::Vec4f>  _sunDiffuse;

    osg::ref_ptr<osg::Light> _light;

    osg::ref_ptr<osg::Group> _scene;

    SCENE_TYPE _sceneType;

    osg::ref_ptr<SkyDome> _skyDome;
};

#endif /* SCENEMODEL_H_ */
