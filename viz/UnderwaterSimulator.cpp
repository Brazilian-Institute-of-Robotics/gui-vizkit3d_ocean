#include <iostream>
#include "UnderwaterSimulator.hpp"

using namespace vizkit3d;

struct UnderwaterSimulator::Data {
    // Copy of the value given to updateDataIntern.
    //
    // Making a copy is required because of how OSG works
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
    // Geode is a common node used for vizkit3d plugins. It allows to display
    // "arbitrary" geometries
    return new osg::Geode();
}

void UnderwaterSimulator::updateMainNode ( osg::Node* node )
{
    osg::Geode* geode = static_cast<osg::Geode*>(node);
    // Update the main node using the data in p->data
}

void UnderwaterSimulator::updateDataIntern(base::Vector3d const& value)
{
    p->data = value;
    std::cout << "got new sample data" << std::endl;
}

//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(UnderwaterSimulator)

