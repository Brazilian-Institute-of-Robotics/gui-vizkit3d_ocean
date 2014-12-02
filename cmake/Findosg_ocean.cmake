find_path(
    OSGOCEAN_INCLUDE_DIR
    NAMES	osgOcean/OceanScene osgOcean/FFTOceanSurface osgOcean/ShaderManager osgOcean/config.h
    HINTS 	$ENV{CMAKE_INSTALL_PREFIX}/include
)

find_library(
    OSGOCEAN_LIBRARY
    NAMES osgOceanD
    PATHS $ENV{CMAKE_INSTALL_PREFIX}/lib
)


set(OSGOCEAN_FOUND "NO")

if( OSGOCEAN_INCLUDE_DIR AND OSGOCEAN_LIBRARY )
    set( OSGOCEAN_FOUND "YES" )
endif()
