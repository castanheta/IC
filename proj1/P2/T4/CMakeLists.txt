# Minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(audio_processor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(audio_processor main.cpp)

# Link the SndFile library to the executable
target_link_libraries(audio_processor sndfile)
