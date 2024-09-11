cmake_minimum_required(VERSION 3.12)
project(soft_renderer)
set(CXX_STANDARD 17)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_subdirectory(application)

aux_source_directory(. SRCS)
add_executable(soft_renderer ${SRCS})

target_link_libraries( soft_renderer ${OpenCV_LIBS} )
target_link_libraries( soft_renderer application_lib)