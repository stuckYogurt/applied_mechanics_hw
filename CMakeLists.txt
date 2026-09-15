cmake_minimum_required(VERSION 3.28)
project(applied_mechs)

set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME applied_mechs)

include(ExternalProject)


add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(lib/sofa)