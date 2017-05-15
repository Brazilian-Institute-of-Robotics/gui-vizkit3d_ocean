# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
find_package(Rock)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

rock_init(vizkit3d_ocean 0.1)
rock_find_qt4()
rock_standard_layout()

install(DIRECTORY resources/textures/
    DESTINATION share/vizkit3d_ocean/textures)
    
install(DIRECTORY resources/shaders/
    DESTINATION share/vizkit3d_ocean/shaders)
