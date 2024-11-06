cmake_minimum_required(VERSION 3.10)

# Set project name
project(BitStreamEncoderDecoder)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the BitStream source file
set(SOURCE_FILES BitStream.cpp)

# Add the encoder executable
add_executable(encoder encoder.cpp ${SOURCE_FILES})

# Add the decoder executable
add_executable(decoder decoder.cpp ${SOURCE_FILES})

# Include directories (if you have a separate include directory, add it here)
# target_include_directories(encoder PRIVATE include)
# target_include_directories(decoder PRIVATE include)

# Set any additional compiler flags for optimization
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(encoder PRIVATE -O2)
    target_compile_options(decoder PRIVATE -O2)
endif()

