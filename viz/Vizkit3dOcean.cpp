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
	Vizkit3DWidget * widget = dynamic_cast<Vizkit3DWidget *>(this->parent());

	osgViewer::View *view = widget->getView(0);
    osg::Camera* camera = view->getCamera();

    osg::Group *root = widget->getRootNode();

    osg::ref_ptr<osg::StateSet> state = root->getOrCreateStateSet();
    state->setGlobalDefaults();
    root->setDataVariance(osg::Object::DYNAMIC);

	osg::Vec3f eye(0.f,0.f,20.f);
	osg::Vec3f centre = eye + osg::Vec3f(0.f,1.f,0.f);
	osg::Vec3f up(0.f, 0.f, 1.f);
	camera->setViewMatrixAsLookAt( eye, centre, up );

	osgGA::TrackballManipulator* tb = new osgGA::TrackballManipulator;
	tb->setHomePosition( osg::Vec3f(0.f,0.f,20.f), osg::Vec3f(0.f,20.f,20.f), osg::Vec3f(0,0,1) );
	view->setCameraManipulator( tb );

	_sceneModel = SceneModel::create();
    return _sceneModel->getScene();
}

void Vizkit3dOcean::updateMainNode ( osg::Node* node )
{
    osg::Geode* geode = static_cast<osg::Geode*>(node);
}

void Vizkit3dOcean::updateDataIntern(base::Vector3d const& value)
{
    p->data = value;
    std::cout << "got new sample data" << std::endl;
}

VizkitQtPlugin(Vizkit3dOcean)

