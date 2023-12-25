cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(advent)

add_executable(advent main.cpp)
target_include_directories(advent PRIVATE "${PROJECT_SOURCE_DIR}/../utils" "/usr/local/include")

find_library(z3_location NAMES libz3.dylib)
message(STATUS ${z3_location})

add_library(z3_location STATIC IMPORTED)
set_target_properties(z3_location PROPERTIES IMPORTED_LOCATION ${z3_location})

target_link_libraries(advent z3_location )
