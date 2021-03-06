#ifndef vizkit3d_ocean_Ocean_H
#define vizkit3d_ocean_Ocean_H

#include <boost/noncopyable.hpp>
#include <vizkit3d/EnvPluginBase.hpp>
#include <osg/Geode>
#include <base/Eigen.hpp>

namespace osg
{
    class TextureCubeMap;
    class Node;
}

namespace osgOcean
{
    class FFTOceanTechnique;
    class OceanScene;
};

namespace vizkit3d_ocean
{
    class SkyDome;
}

namespace vizkit3d
{

struct OceanParameters
{
    OceanParameters() 
        : surfDirty(true)
        , surfEndless(true)
        , surfWaveScale(1e-7)
        , surfDepth(1000)
        , surfWindDirection(1, 2)
        , surfWindSpeed(3.0)
        , surfReflectionDamping(0.2)
        , surfIsChoppy(true)
        , surfChoppyFactor(-2.5)
        , surfCrestFoam(true)
        , surfCrestFoamHeight(2.2)
        , surfFoamBottomHeight(2.2)
        , surfFoamTopHeight(2.2)

        , sceneDirty(true)
        , airFogColor(199, 226, 255)
        , airFogDensity(0.012)
        , sunPosition(326, 1212, 1275)
        , sunDiffuseColor(191, 191, 191)
        , uwFogColor(27, 57, 109)
        , uwFogDensity(0.02)
        , uwAttenuation(0.015, 0.0075, 0.005)
        , uwDiffuseColor(27, 57, 109)
        , glareAttenuation(0.8)

        , reflections(true)
        , refractions(false)
        , heightmap(true)
        , godRays(true)
        , silt(true)
        , underwaterDOF(false)
        , underwaterScattering(true)
        , distortion(true)
        , glare(false)
    {
    }

    /// Ocean surface parameters
    bool      surfDirty;
    bool      surfEndless;
    float     surfWaveScale;
    float     surfDepth;
    QVector2D surfWindDirection;
    float     surfWindSpeed;
    float     surfReflectionDamping;
    bool      surfIsChoppy;
    float     surfChoppyFactor;
    bool      surfCrestFoam;
    float     surfCrestFoamHeight;
    float     surfFoamBottomHeight;
    float     surfFoamTopHeight;

    /// Ocean scene parameters
    bool      sceneDirty;
    QColor    airFogColor;
    float     airFogDensity;
    QVector3D sunPosition;
    QColor    sunDiffuseColor;
    QColor    uwFogColor;
    float     uwFogDensity;
    QVector3D uwAttenuation;
    QColor    uwDiffuseColor;
    float     glareAttenuation;
    bool      reflections;
    bool      refractions;
    bool      heightmap;
    bool      godRays;
    bool      silt;
    bool      underwaterDOF;
    bool      underwaterScattering;
    bool      distortion;
    bool      glare;

};

class Ocean
	: public vizkit3d::EnvPluginBase
	, boost::noncopyable
{

        Q_OBJECT


public:
        enum CHILDREN
        {
            MAIN_CHILD_SURFACE,
            MAIN_CHILD_SCENE,
            MAIN_CHILD_SKYDOME_TRANSFORM,
            MAIN_CHILD_LIGHT
        };

	explicit Ocean(const OceanParameters& ocean_parameters = OceanParameters());
	~Ocean();

protected:
	virtual osg::ref_ptr<osg::Node> createMainNode();
	virtual void updateMainNode(osg::Node* node);
	virtual void updateDataIntern(base::Vector3d const& plan);

        void loadCubeMapImages(QString dir);
        osg::TextureCubeMap* createCubeMap();
        void updateCubeMap(osg::TextureCubeMap* cubeMap);
        osgOcean::FFTOceanTechnique* createSurface();
        void updateSurface(osgOcean::FFTOceanTechnique* surface,
                osg::TextureCubeMap* cubeMap);
        osgOcean::OceanScene* createScene(osgOcean::FFTOceanTechnique* surface);
        void updateScene(osgOcean::OceanScene* scene);
        osg::LightSource* createLight();
        void updateLight(osg::LightSource* light);
        vizkit3d_ocean::SkyDome* createSkyDome(osg::TextureCubeMap* cubeMap);
        void updateSkyDome(vizkit3d_ocean::SkyDome* dome, osgOcean::OceanScene* scene);

        osg::ref_ptr<osg::Group> getRefNode();

        void setupShader(osg::Node *node);

private:

        osg::ref_ptr<osg::Group> ref_node;

        bool      cubeMapDirty;
        QString   cubeMapPath;
        osg::ref_ptr<osg::Image> cubeMapImages[6];
        QColor    lightColor;

        /// Ocean surface parameters
        bool      surfDirty;
        bool      surfEndless;
        float     surfWaveScale;
        float     surfDepth;
        QVector2D surfWindDirection;
        float     surfWindSpeed;
        float     surfReflectionDamping;
        bool      surfIsChoppy;
        float     surfChoppyFactor;
        bool      surfCrestFoam;
        float     surfCrestFoamHeight;
        float     surfFoamBottomHeight;
        float     surfFoamTopHeight;

        /// Ocean scene parameters
        bool      sceneDirty;
        QColor    airFogColor;
        float     airFogDensity;
        QVector3D sunPosition;
        QColor    sunDiffuseColor;
        QColor    uwFogColor;
        float     uwFogDensity;
        QVector3D uwAttenuation;
        QColor    uwDiffuseColor;
        float     glareAttenuation;
        bool      reflections;
        bool      refractions;
        bool      heightmap;
        bool      godRays;
        bool      silt;
        bool      underwaterDOF;
        bool      underwaterScattering;
        bool      distortion;
        bool      glare;
};
}
#endif
