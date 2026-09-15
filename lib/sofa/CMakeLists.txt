file(GLOB_RECURSE execs_srcs *.c)
add_library(sofa ${execs_srcs})
file(GLOB_RECURSE execs_hs *.h)
target_include_directories(sofa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC sofa)
