cmake_minimum_required(VERSION 3.10)

# Set project name
project(BitStreamEncoderDecoder)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
set(SOURCE_FILES
    BitStream.cpp
    GolombCoding.cpp
)

# Include directories
# If you have headers in a separate include/ directory:
# target_include_directories(encoder PRIVATE include)
# target_include_directories(decoder PRIVATE include)
# target_include_directories(golombTest PRIVATE include)

# Add executables
add_executable(encoder encoder.cpp ${SOURCE_FILES})
add_executable(decoder decoder.cpp ${SOURCE_FILES})
add_executable(golombTest GolombTest.cpp ${SOURCE_FILES})

# Apply compiler options globally
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O2) # Apply to all targets
endif()

