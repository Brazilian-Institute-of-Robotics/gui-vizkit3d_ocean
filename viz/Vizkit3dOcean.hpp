#ifndef vizkit3d_ocean_Vizkit3dOcean_H
#define vizkit3d_ocean_Vizkit3dOcean_H

#include <boost/noncopyable.hpp>
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <osg/Geode>
#include <base/Eigen.hpp>
#include <underwater_simulator/SceneModel.h>

namespace vizkit3d
{
class Vizkit3dOcean
	: public vizkit3d::Vizkit3DPlugin<base::Vector3d>
	, boost::noncopyable
{

Q_OBJECT

public:

	Vizkit3dOcean();
	~Vizkit3dOcean();

	Q_INVOKABLE void updateData(base::Vector3d const &sample)
	{
		vizkit3d::Vizkit3DPlugin<base::Vector3d>::updateData(sample);
	}

protected:
	virtual osg::ref_ptr<osg::Node> createMainNode();
	virtual void updateMainNode(osg::Node* node);
	virtual void updateDataIntern(base::Vector3d const& plan);

private:
	struct Data;
	Data* p;

	SceneModel *_sceneModel;

};
}
#endif
