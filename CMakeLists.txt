cmake_minimum_required(VERSION 3.12)
project(soft_renderer)
set(CXX_STANDARD 20)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ./base )
include_directories( ./math )
include_directories( ./application )
include_directories( ./gpu )
include_directories( ./color )
include_directories( ./raster )

add_subdirectory(application)
add_subdirectory(gpu)
add_subdirectory(raster)
add_subdirectory(color)

# math_test
add_executable(math_test test/math_test.cpp)

# rasterize_line_bresenham_test
add_executable(rasterize_line_bresenham_test test/rasterize_line_bresenham_test.cpp)
target_link_libraries( rasterize_line_bresenham_test ${OpenCV_LIBS} )
target_link_libraries( rasterize_line_bresenham_test application_lib )
target_link_libraries( rasterize_line_bresenham_test gpu_lib )
target_link_libraries( rasterize_line_bresenham_test raster_lib )
target_link_libraries( rasterize_line_bresenham_test color_lib )

# rasterize_line_aa_test
add_executable(rasterize_line_aa_test test/rasterize_line_aa_test.cpp)
target_link_libraries( rasterize_line_aa_test ${OpenCV_LIBS} )
target_link_libraries( rasterize_line_aa_test application_lib )
target_link_libraries( rasterize_line_aa_test gpu_lib )
target_link_libraries( rasterize_line_aa_test raster_lib )
target_link_libraries( rasterize_line_aa_test color_lib )


# soft_renderer
add_executable(soft_renderer main.cpp)
target_link_libraries( soft_renderer ${OpenCV_LIBS} )
target_link_libraries( soft_renderer application_lib )
target_link_libraries( soft_renderer gpu_lib )
target_link_libraries( soft_renderer raster_lib )
target_link_libraries( soft_renderer color_lib )


