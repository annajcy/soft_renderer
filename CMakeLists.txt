cmake_minimum_required(VERSION 3.12)
project(soft_renderer)
set(CXX_STANDARD 17)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ./base )

add_subdirectory(application)
add_subdirectory(gpu)

aux_source_directory(. SRCS)
add_executable(soft_renderer ${SRCS})

target_link_libraries( soft_renderer ${OpenCV_LIBS} )
target_link_libraries( soft_renderer application_lib )
target_link_libraries( soft_renderer gpu_lib )