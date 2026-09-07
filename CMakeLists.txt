cmake_minimum_required(VERSION 3.28)
project(applied_mechs)

set(CMAKE_CXX_STANDARD 20)

add_executable(applied_mechs main.cpp
        src/time-keeping.cpp)
target_include_directories(applied_mechs PUBLIC ${include/})