cmake_minimum_required(VERSION 3.28)
project(applied_mechs)
set(CMAKE_CXX_STANDARD 20)

include(ExternalProject)


add_executable(applied_mechs main.cpp)

set(SOFA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/sofa/include")
set(SOFA_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/sofa/bin")

add_library(sofa STATIC IMPORTED)
set_target_properties(sofa PROPERTIES
        IMPORTED_LOCATION "${SOFA_INSTALL_DIR}/libsofa_c.a"
        INTERFACE_INCLUDE_DIRECTORIES "${SOFA_INCLUDE_DIR}"
)

target_link_libraries(applied_mechs PUBLIC sofa)