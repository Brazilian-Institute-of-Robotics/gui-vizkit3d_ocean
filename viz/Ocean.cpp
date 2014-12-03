#include <iostream>
#include <osgViewer/View>
#include <osgGA/TrackballManipulator>
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <vizkit3d/Ocean.hpp>

using namespace vizkit3d;

struct Ocean::Data {
    base::Vector3d data;
};


Ocean::Ocean()
    : p(new Data)
{
}

Ocean::~Ocean()
{
    delete p;
}

osg::ref_ptr<osg::Node> Ocean::createMainNode()
{
    _sceneModel = SceneModel::create();
    osg::ref_ptr<osg::Group> scene = _sceneModel->getScene();
    osg::ref_ptr<osg::StateSet> state = scene->getOrCreateStateSet();
    state->setGlobalDefaults();
    state->setDataVariance(osg::Object::DYNAMIC);
    return scene;
}

void Ocean::updateMainNode ( osg::Node* node )
{
}

void Ocean::updateDataIntern(base::Vector3d const& value)
{
    p->data = value;
    std::cout << "got new sample data" << std::endl;
}

VizkitQtPlugin(Ocean)

