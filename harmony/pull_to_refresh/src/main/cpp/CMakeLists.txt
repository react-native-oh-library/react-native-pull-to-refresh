cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

# file(GLOB rnoh_nested_scroll_SRC CONFIGURE_DEPENDS *.cpp)
# add_library(rnoh_nested_scroll SHARED ${rnoh_nested_scroll_SRC})
# target_include_directories(rnoh_nested_scroll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(rnoh_nested_scroll PUBLIC rnoh)

set(rnoh_pull_to_refresh_generated_dir "${CMAKE_CURRENT_SOURCE_DIR}/generated")
file(GLOB_RECURSE rnoh_pull_to_refresh_generated_SRC "${rnoh_pull_to_refresh_generated_dir}/**/*.cpp")
file(GLOB rnoh_pull_to_refresh_SRC CONFIGURE_DEPENDS *.cpp)
add_library(rnoh_pull_to_refresh SHARED ${rnoh_pull_to_refresh_SRC} ${rnoh_pull_to_refresh_generated_SRC})
target_include_directories(rnoh_pull_to_refresh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${rnoh_pull_to_refresh_generated_dir})
target_link_libraries(rnoh_pull_to_refresh PUBLIC rnoh)