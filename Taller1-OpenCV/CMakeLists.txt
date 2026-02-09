cmake_minimum_required(VERSION 3.12)
project(opencv_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_executable(test_camera src/test_camera.cpp)
target_link_libraries(test_camera PRIVATE ${OpenCV_LIBS})

add_executable(image_converter src/image_converter.cpp)
target_link_libraries(image_converter PRIVATE ${OpenCV_LIBS})
