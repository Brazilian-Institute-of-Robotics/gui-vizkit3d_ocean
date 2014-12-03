find_path(
    OSGOCEAN_INCLUDE_DIR
    NAMES	osgOcean/OceanScene osgOcean/FFTOceanSurface osgOcean/ShaderManager
    HINTS 	$ENV{CMAKE_INSTALL_PREFIX}/include
)

find_library(
    OSGOCEAN_LIBRARY
    NAMES osgOcean
    PATHS $ENV{CMAKE_INSTALL_PREFIX}/lib
)


set(OSGOCEAN_FOUND "NO")

if( OSGOCEAN_INCLUDE_DIR AND OSGOCEAN_LIBRARY )
    set( OSGOCEAN_FOUND "YES" )
endif()
