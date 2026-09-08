cmake_minimum_required(VERSION 3.28)
project(applied_mechs)
set(CMAKE_CXX_STANDARD 20)

include(ExternalProject)


add_executable(applied_mechs main.cpp
        src/time-keeping.cpp)

set(SOFA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/sofa/include")
set(SOFA_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/sofa/src")

add_library(sofa STATIC IMPORTED)
set_target_properties(sofa PROPERTIES
        IMPORTED_LOCATION "${LIB_INSTALL_DIR}/libsofa_c.a"
        INTERFACE_INCLUDE_DIRECTORIES "${LIB_INCLUDE_DIR}"
)
