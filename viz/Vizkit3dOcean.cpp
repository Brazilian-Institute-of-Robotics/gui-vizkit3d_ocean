#include <iostream>
#include <osgViewer/View>
#include <osgGA/TrackballManipulator>
#include <vizkit3d/Vizkit3DWidget.hpp>
#include "Vizkit3dOcean.hpp"

using namespace vizkit3d;

struct Vizkit3dOcean::Data {
    base::Vector3d data;
};


Vizkit3dOcean::Vizkit3dOcean()
    : p(new Data)
{
}

Vizkit3dOcean::~Vizkit3dOcean()
{
    delete p;
}

osg::ref_ptr<osg::Node> Vizkit3dOcean::createMainNode()
{
    _sceneModel = SceneModel::create();
    osg::ref_ptr<osg::Group> scene = _sceneModel->getScene();
    osg::ref_ptr<osg::StateSet> state = scene->getOrCreateStateSet();
    state->setGlobalDefaults();
    state->setDataVariance(osg::Object::DYNAMIC);
    return scene;
}

void Vizkit3dOcean::updateMainNode ( osg::Node* node )
{
}

void Vizkit3dOcean::updateDataIntern(base::Vector3d const& value)
{
    p->data = value;
    std::cout << "got new sample data" << std::endl;
}

VizkitQtPlugin(Vizkit3dOcean)

