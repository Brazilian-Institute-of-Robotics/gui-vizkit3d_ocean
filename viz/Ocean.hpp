#ifndef vizkit3d_ocean_Ocean_H
#define vizkit3d_ocean_Ocean_H

#include <boost/noncopyable.hpp>
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <osg/Geode>
#include <base/Eigen.hpp>
#include <vizkit3d_ocean/SceneModel.h>

namespace vizkit3d
{
class Ocean
	: public vizkit3d::Vizkit3DPlugin<base::Vector3d>
	, boost::noncopyable
{

Q_OBJECT

public:

	Ocean();
	~Ocean();

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
