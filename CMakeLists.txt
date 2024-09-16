cmake_minimum_required(VERSION 3.12)
project(soft_renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( base )
include_directories( math )
include_directories( application )
include_directories( gpu )

file(GLOB copy_resources "assets")
file(COPY ${copy_resources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

# soft_renderer
add_executable(soft_renderer main.cpp)
target_link_libraries( soft_renderer ${OpenCV_LIBS} )
target_link_libraries( soft_renderer application_lib )
target_link_libraries( soft_renderer gpu_lib )


