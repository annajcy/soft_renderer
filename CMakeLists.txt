cmake_minimum_required(VERSION 3.12)
project(soft_renderer)
set(CXX_STANDARD 20)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( base )
include_directories( math )
include_directories( application )
include_directories( gpu )
include_directories( color )
include_directories( raster )

add_subdirectory(application)
add_subdirectory(gpu)
add_subdirectory(raster)
add_subdirectory(color)

# soft_renderer
add_executable(soft_renderer main.cpp)
target_link_libraries( soft_renderer ${OpenCV_LIBS} )
target_link_libraries( soft_renderer application_lib )
target_link_libraries( soft_renderer gpu_lib )
target_link_libraries( soft_renderer raster_lib )
target_link_libraries( soft_renderer color_lib )


