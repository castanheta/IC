cmake_minimum_required(VERSION 3.10)

# Project name
project(image_processor)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Specify the executable target
add_executable(image_processor main.cpp)

# Link the OpenCV libraries to the executable
target_link_libraries(image_processor ${OpenCV_LIBS})

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
