# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Set the project name and specify the language
project(text_processor)

# Specify the C++ standard to use (optional but recommended)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target (the program to be built)
# In this case, 'src/main.cpp' is the source file
add_executable(text_processor main.cpp)

