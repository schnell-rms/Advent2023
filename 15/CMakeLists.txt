cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(advent)

add_executable(advent main.cpp)
target_include_directories(advent PRIVATE "${PROJECT_SOURCE_DIR}/../utils")
