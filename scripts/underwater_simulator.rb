require 'vizkit'
Orocos.initialize

view3d = Vizkit.vizkit3d_widget
view3d.show()
Vizkit.exec()
