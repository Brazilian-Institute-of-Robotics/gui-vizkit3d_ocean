#include <iostream>
#include <osgViewer/View>
#include <osgGA/TrackballManipulator>
#include <vizkit3d/Vizkit3DWidget.hpp>
#include "UnderwaterSimulator.hpp"

using namespace vizkit3d;

struct UnderwaterSimulator::Data {
    base::Vector3d data;
};


UnderwaterSimulator::UnderwaterSimulator()
    : p(new Data)
{
}

UnderwaterSimulator::~UnderwaterSimulator()
{
    delete p;
}

osg::ref_ptr<osg::Node> UnderwaterSimulator::createMainNode()
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

void UnderwaterSimulator::updateMainNode ( osg::Node* node )
{
    osg::Geode* geode = static_cast<osg::Geode*>(node);
}

void UnderwaterSimulator::updateDataIntern(base::Vector3d const& value)
{
    p->data = value;
}

//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(UnderwaterSimulator)

