#ifndef underwater_simulator_UnderwaterSimulator_H
#define underwater_simulator_UnderwaterSimulator_H

#include <boost/noncopyable.hpp>
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <osg/Geode>
#include <base/eigen.h>

namespace vizkit3d
{
    class UnderwaterSimulator
        : public vizkit3d::Vizkit3DPlugin<base::Vector3d>
        , boost::noncopyable
    {
    Q_OBJECT
    public:
        UnderwaterSimulator();
        ~UnderwaterSimulator();

    Q_INVOKABLE void updateData(base::Vector3d const &sample)
    {vizkit3d::Vizkit3DPlugin<base::Vector3d>::updateData(sample);}

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
        virtual void updateMainNode(osg::Node* node);
        virtual void updateDataIntern(base::Vector3d const& plan);
        
    private:
        struct Data;
        Data* p;
    };
}
#endif
