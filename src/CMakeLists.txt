include_directories(${OSGOCEAN_INCLUDE_DIR})

rock_library(vizkit3d_ocean
    SOURCES 
        SceneModel.cpp 
        SkyDome.cpp 
        SphereSegment.cpp
    HEADERS 
        SceneModel.h 
        SkyDome.h 
        SphereSegment.h
    DEPS_CMAKE 
        osg_ocean
    LIBS 
        ${OSGOCEAN_LIBRARY}
)