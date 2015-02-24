require 'vizkit'
Orocos.initialize

current_path = File.expand_path(File.dirname(__FILE__))
  
view3d = Vizkit.vizkit3d_widget

ocean = Vizkit.default_loader.Ocean

#view3d.setGrid(false)
view3d.setCameraManipulator("Trackball")
view3d.setEnvironmentPlugin(ocean)
view3d.setAxes(false);
view3d.setTransformer(false);

view3d.setCameraEye(-10, -2.5, -15)
view3d.setCameraLookAt(0, 0, -15)
view3d.setCameraUp(0, 0, 1)
view3d.show

flatfish1 = Vizkit.default_loader.RobotVisualization
flatfish1.modelFile = "#{current_path}/robots/flatfish.urdf"
flatfish1.setPose(Qt::Vector3D.new(0,1.5,-15),Qt::Quaternion::fromAxisAndAngle(0,0,0,0))
  
girona = Vizkit.default_loader.RobotVisualization
girona.modelFile = "#{current_path}/robots/GIRONA500ARM5.urdf"
girona.setPose(Qt::Vector3D.new(0,-1.5,-15),Qt::Quaternion::fromAxisAndAngle(1,0,0,180))
  
Vizkit.exec
