cmake_minimum_required(VERSION 3.10)

project(eschaton)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(eschaton tests.cpp)

target_include_directories(eschaton PUBLIC
    "${PROJECT_SOURCE_DIR}/../include"
    "${PROJECT_SOURCE_DIR}/../thirdparty/include"
)
