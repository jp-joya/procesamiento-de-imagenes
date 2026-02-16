cmake_minimum_required(VERSION 3.16)

project(taller3_backgrounds LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_library(background_models src/background_models.cpp)
target_include_directories(
    background_models
    PRIVATE ${OpenCV_INCLUDE_DIRS}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(background_models PRIVATE ${OpenCV_LIBS})

add_executable(frame src/frame.cpp)
target_link_libraries(frame PRIVATE background_models ${OpenCV_LIBS})

add_executable(running_average_manual src/running_average_manual.cpp)
target_link_libraries(running_average_manual PRIVATE background_models ${OpenCV_LIBS})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE background_models ${OpenCV_LIBS})
