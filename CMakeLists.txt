cmake_minimum_required(VERSION 3.25)
project(flaschentype)
include(FetchContent)

#FetchContent_Declare(
#    freetype
#    URL https://gitlab.freedesktop.org/freetype/freetype/-/archive/VER-2-13-2/freetype-VER-2-13-2.tar.gz
#)
#FetchContent_MakeAvailable(freetype)

find_package(Freetype REQUIRED)
add_library(flaschentype STATIC "flaschentype.cpp")
target_include_directories(flaschentype PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(flaschentype PUBLIC Freetype::Freetype)