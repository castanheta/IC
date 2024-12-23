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
    ImageCodec.cpp
    VideoCodecIntra.cpp
)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executables
add_executable(encoder encoder.cpp ${SOURCE_FILES})
add_executable(decoder decoder.cpp ${SOURCE_FILES})
add_executable(golombTest GolombTest.cpp ${SOURCE_FILES})
add_executable(imageTest ImageCodecTest.cpp ${SOURCE_FILES})
add_executable(videoIntraTest VideoCodecIntraTest.cpp ${SOURCE_FILES})

# Link OpenCV libraries
target_link_libraries(encoder ${OpenCV_LIBS})
target_link_libraries(decoder ${OpenCV_LIBS})
target_link_libraries(golombTest ${OpenCV_LIBS})
target_link_libraries(imageTest ${OpenCV_LIBS})
target_link_libraries(videoIntraTest ${OpenCV_LIBS})

# Apply compiler options globally
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O2) # Apply to all targets
endif()

