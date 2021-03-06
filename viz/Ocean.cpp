#include <vizkit3d/Ocean.hpp>
#include <vizkit3d_ocean/SkyDome.hpp>

#include <osgOcean/FFTOceanSurface>
#include <osgOcean/OceanScene>
#include <osgOcean/ShaderManager>
#include <osgDB/FileUtils>
#include <osg/LightSource>

using namespace vizkit3d;
using namespace osg;
using namespace osgOcean;

class CameraTrackCallback: public osg::NodeCallback
{
public:
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        if( nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR )
        {
            osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(nv);
            osg::Vec3f centre,up,eye;
            // get MAIN camera eye,centre,up
            cv->getRenderStage()->getCamera()->getViewMatrixAsLookAt(eye,centre,up);
            // update position
            osg::MatrixTransform* mt = static_cast<osg::MatrixTransform*>(node);
            mt->setMatrix( osg::Matrix::translate( eye.x(), eye.y(), mt->getMatrix().getTrans().z() ) );
        }

        traverse(node, nv);
    }
};

static osg::Vec4f qt2osg(QColor const& color)
{
    return osg::Vec4f(color.redF(), color.greenF(), color.blueF(), 1.0f);
}
static osg::Vec2f qt2osg(QVector2D const& v)
{
    return osg::Vec2f(v.x(), v.y());
}
static osg::Vec3f qt2osg(QVector3D const& v)
{
    return osg::Vec3f(v.x(), v.y(), v.z());
}


Ocean::Ocean(const OceanParameters& ocean_parameters)
    : cubeMapDirty(false)
    , cubeMapPath("vizkit3d_ocean/textures/sky_clear")
    , lightColor(105, 138, 174)
{
    surfDirty = ocean_parameters.surfDirty;
    surfEndless = ocean_parameters.surfEndless;
    surfWaveScale = ocean_parameters.surfWaveScale;
    surfDepth = ocean_parameters.surfDepth;
    surfWindDirection = ocean_parameters.surfWindDirection;
    surfWindSpeed = ocean_parameters.surfWindSpeed;
    surfReflectionDamping = ocean_parameters.surfReflectionDamping;
    surfIsChoppy = ocean_parameters.surfIsChoppy;
    surfChoppyFactor = ocean_parameters.surfChoppyFactor;
    surfCrestFoam = ocean_parameters.surfCrestFoam;
    surfCrestFoamHeight = ocean_parameters.surfCrestFoamHeight;
    surfFoamBottomHeight = ocean_parameters.surfFoamBottomHeight;
    surfFoamTopHeight = ocean_parameters.surfFoamTopHeight;

    sceneDirty = ocean_parameters.sceneDirty;
    airFogColor = ocean_parameters.airFogColor;
    airFogDensity = ocean_parameters.airFogDensity;
    sunPosition = ocean_parameters.sunPosition;
    sunDiffuseColor = ocean_parameters.sunDiffuseColor;
    uwFogColor = ocean_parameters.uwFogColor;
    uwFogDensity = ocean_parameters.uwFogDensity;
    uwAttenuation = ocean_parameters.uwAttenuation;
    uwDiffuseColor = ocean_parameters.uwDiffuseColor;
    glareAttenuation = ocean_parameters.glareAttenuation;

    reflections = ocean_parameters.reflections;
    refractions = ocean_parameters.refractions;
    heightmap = ocean_parameters.heightmap;
    godRays = ocean_parameters.godRays;
    silt = ocean_parameters.silt;
    underwaterDOF = ocean_parameters.underwaterDOF;
    underwaterScattering = ocean_parameters.underwaterScattering;
    distortion = ocean_parameters.distortion;
    glare = ocean_parameters.glare;

    loadCubeMapImages(cubeMapPath);
    ref_node = new osg::Group;
}

Ocean::~Ocean()
{
}

ref_ptr<Group> Ocean::getRefNode()
{
    return ref_node;
}

void Ocean::setupShader(osg::Node* node)
{
    static const char model_vertex[] = "vizkit3d_ocean/shaders/default_scene.vert";
    static const char model_fragment[] = "vizkit3d_ocean/shaders/default_scene.frag";

    osg::ref_ptr<osg::Program> program =
        osgOcean::ShaderManager::instance().createProgram(
                "object_shader", model_vertex, model_fragment, "", "");

    if (program.valid()) {
        program->addBindAttribLocation("aTangent", 6);
        node->getOrCreateStateSet()->setAttributeAndModes(program,
                osg::StateAttribute::ON);
        node->getStateSet()->addUniform(
                new osg::Uniform("uOverlayMap", 1));
        node->getStateSet()->addUniform(
                new osg::Uniform("uNormalMap", 2));
    }
}

ref_ptr<Node> Ocean::createMainNode()
{
    TextureCubeMap* cubeMap = createCubeMap();
    updateCubeMap(cubeMap);
    FFTOceanTechnique* surface = createSurface();
    updateSurface(surface, cubeMap);
    OceanScene* scene = createScene(surface);
    updateScene(scene);
    vizkit3d_ocean::SkyDome* dome = createSkyDome(cubeMap);
    updateSkyDome(dome, scene);
    LightSource* light = createLight();
    updateLight(light);

    Group* mainNode = new Group;
    mainNode->addChild(scene);
    scene->addChild(ref_node);
    mainNode->addChild(light);

    osg::ref_ptr<osg::StateSet> state = mainNode->getOrCreateStateSet();
    state->setGlobalDefaults();
    state->setDataVariance(osg::Object::DYNAMIC);

    // add a pat to track the camera
    MatrixTransform* transform = new MatrixTransform;
    transform->setDataVariance( Object::DYNAMIC );
    transform->setMatrix(Matrixf::translate( Vec3f(0.f, 0.f, 0.f) ));
    transform->setCullCallback( new CameraTrackCallback );
    transform->addChild(dome);
    scene->addChild(transform);

    setupShader(ref_node);

    ref_node->setNodeMask(scene->getNormalSceneMask() |
            scene->getReflectedSceneMask() |
            scene->getRefractedSceneMask());

    osgOcean::ShaderManager::instance().enableShaders(true);

    cubeMapDirty = false;
    surfDirty = false;
    sceneDirty = false;
    return mainNode;
}

void Ocean::loadCubeMapImages(QString dir)
{
    std::string filenames[6];
    filenames[0] = osgDB::findDataFile(dir.toStdString() + "/west.png");
    filenames[1] = osgDB::findDataFile(dir.toStdString() + "/east.png");
    filenames[2] = osgDB::findDataFile(dir.toStdString() + "/up.png");
    filenames[3] = osgDB::findDataFile(dir.toStdString() + "/down.png");
    filenames[4] = osgDB::findDataFile(dir.toStdString() + "/south.png");
    filenames[5] = osgDB::findDataFile(dir.toStdString() + "/north.png");

    ref_ptr<Image> images[6];
    for (int i = 0; i < 6; ++i)
    {
        if (filenames[i].empty())
            throw std::runtime_error("cannot find required cubemap texture in " + dir.toStdString());
        images[i] = osgDB::readImageFile( filenames[i] );
    }

    for (int i = 0; i < 6; ++i)
        cubeMapImages[i] = images[i];
}

TextureCubeMap* Ocean::createCubeMap()
{
    TextureCubeMap* cubeMap = new TextureCubeMap;
    cubeMap->setInternalFormat(GL_RGBA);
    cubeMap->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
    cubeMap->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
    cubeMap->setWrap(Texture::WRAP_S, Texture::CLAMP_TO_EDGE);
    cubeMap->setWrap(Texture::WRAP_T, Texture::CLAMP_TO_EDGE);
    return cubeMap;
}

void Ocean::updateCubeMap(TextureCubeMap* cubeMap)
{
    cubeMap->setImage(TextureCubeMap::NEGATIVE_X, cubeMapImages[0]);
    cubeMap->setImage(TextureCubeMap::POSITIVE_X, cubeMapImages[1]);
    cubeMap->setImage(TextureCubeMap::NEGATIVE_Y, cubeMapImages[2]);
    cubeMap->setImage(TextureCubeMap::POSITIVE_Y, cubeMapImages[3]);
    cubeMap->setImage(TextureCubeMap::NEGATIVE_Z, cubeMapImages[4]);
    cubeMap->setImage(TextureCubeMap::POSITIVE_Z, cubeMapImages[5]);
}

osgOcean::FFTOceanTechnique* Ocean::createSurface()
{
    osgOcean::FFTOceanTechnique* surface =
        new osgOcean::FFTOceanSurface( 64, 256, 17,
                qt2osg(surfWindDirection),
                surfWindSpeed,
                surfDepth,
                surfReflectionDamping,
                surfWaveScale,
                surfIsChoppy,
                surfChoppyFactor,
                10.f, 256 );
    return surface;
}

void Ocean::updateSurface(osgOcean::FFTOceanTechnique* surface, TextureCubeMap* cubeMap)
{
    surface->setEnvironmentMap( cubeMap );
    surface->setFoamBottomHeight( surfFoamBottomHeight );
    surface->setFoamTopHeight( surfFoamTopHeight );
    surface->enableCrestFoam(surfCrestFoam);
    surface->setLightColor(qt2osg(lightColor));
    surface->enableEndlessOcean(surfEndless);
}

osgOcean::OceanScene* Ocean::createScene(osgOcean::FFTOceanTechnique* surface)
{
    osgOcean::OceanScene* scene =
        new osgOcean::OceanScene(surface);

    // Create and add fake texture for use with nodes without any texture
    // since the OceanScene default scene shader assumes that texture unit
    // 0 is used as a base texture map.
    Image * image = new Image();
    image->allocateImage( 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE );
    *(Vec4ub*)image->data() = Vec4ub( 0xFF, 0xFF, 0xFF, 0xFF );

    Texture2D* fakeTex = new Texture2D( image );
    fakeTex->setWrap(Texture2D::WRAP_S,Texture2D::REPEAT);
    fakeTex->setWrap(Texture2D::WRAP_T,Texture2D::REPEAT);
    fakeTex->setFilter(Texture2D::MIN_FILTER,Texture2D::NEAREST);
    fakeTex->setFilter(Texture2D::MAG_FILTER,Texture2D::NEAREST);

    StateSet* stateset = scene->getOrCreateStateSet();
    stateset->setTextureAttribute(0,fakeTex,StateAttribute::ON);
    stateset->setTextureMode(0,GL_TEXTURE_1D,StateAttribute::OFF);
    stateset->setTextureMode(0,GL_TEXTURE_2D,StateAttribute::ON);
    stateset->setTextureMode(0,GL_TEXTURE_3D,StateAttribute::OFF);

    return scene;
}

void Ocean::updateScene(osgOcean::OceanScene* scene)
{
    scene->setLightID(0);
    scene->enableReflections(reflections);
    scene->enableRefractions(refractions);
    scene->enableHeightmap(heightmap);

    scene->setCylinderSize( 1900.f, 4000.f );

    scene->setAboveWaterFog(airFogDensity, qt2osg(airFogColor) );
    scene->setUnderwaterFog(uwFogDensity, qt2osg(uwFogColor) );
    scene->setUnderwaterDiffuse(qt2osg(uwDiffuseColor));
    scene->setUnderwaterAttenuation(qt2osg(uwAttenuation));

    Vec3f sunDir = qt2osg(-sunPosition);
    sunDir.normalize();
    scene->setSunDirection( sunDir );

    scene->enableGodRays(godRays);
    scene->enableSilt(silt);
    scene->enableUnderwaterDOF(underwaterDOF);
    scene->enableUnderwaterScattering(underwaterScattering);
    scene->enableDistortion(distortion);
    scene->enableGlare(glare);
    scene->setGlareAttenuation(glareAttenuation);
}

LightSource* Ocean::createLight()
{
    LightSource* lightSource = new LightSource();
    lightSource->setLocalStateSetModes();
    lightSource->getLight()->setLightNum(0);
    return lightSource;
}

void Ocean::updateLight(LightSource* lightSource)
{
    osg::Light* light = lightSource->getLight();
    light->setAmbient( osg::Vec4d(0.3f, 0.3f, 0.3f, 1.0f ));
    light->setDiffuse(qt2osg(sunDiffuseColor));
    light->setSpecular(osg::Vec4d( 0.1f, 0.1f, 0.1f, 1.0f ) );

    osg::Vec3f direction(qt2osg(sunPosition));
    direction.normalize();
    light->setPosition( osg::Vec4f(direction, 0.0) );
}

vizkit3d_ocean::SkyDome* Ocean::createSkyDome(TextureCubeMap* cubeMap)
{
    vizkit3d_ocean::SkyDome* skyDome = new vizkit3d_ocean::SkyDome( 1900.f, 16, 16, cubeMap );
    return skyDome;
}

void Ocean::updateSkyDome(vizkit3d_ocean::SkyDome* dome, OceanScene* scene)
{
    dome->setNodeMask(
            scene->getReflectedSceneMask() | 
            scene->getNormalSceneMask()    | 
            scene->getRefractedSceneMask());
}

void Ocean::updateMainNode( Node* node )
{
	std::cout << "Ocean::updateMainNode" << std::endl;
}

void Ocean::updateDataIntern(base::Vector3d const& value)
{
}

VizkitQtPlugin(Ocean)

