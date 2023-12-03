cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(advent)

include_directories("${PROJECT_SOURCE_DIR}")
add_executable(advent main.cpp)
