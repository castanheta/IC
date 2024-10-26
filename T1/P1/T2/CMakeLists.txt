cmake_minimum_required(VERSION 3.10)
project(text_processor)

# Find Boost.Locale package
find_package(Boost 1.65 REQUIRED COMPONENTS locale)

# Add the executable
add_executable(text_processor main.cpp)

# Link Boost.Locale
target_link_libraries(text_processor Boost::locale)

