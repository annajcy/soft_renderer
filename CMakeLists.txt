cmake_minimum_required(VERSION 3.12)
project(soft_renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(ext/assimp)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( base )
include_directories( math )
include_directories( application )
include_directories( gpu )
include_directories( raster )
include_directories( raytrace )
include_directories( framework )
include_directories( rendering )
include_directories( mesh )
include_directories( utils )
include_directories( curve )

include_directories( ext/assimp/include )

file(GLOB copy_resources "assets")
file(COPY ${copy_resources} DESTINATION ${CMAKE_BINARY_DIR})

# soft_renderer
add_executable(soft_renderer main.cpp)
target_link_libraries( soft_renderer ${OpenCV_LIBS} assimp )


