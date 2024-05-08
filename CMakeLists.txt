cmake_minimum_required(VERSION 3.25)
project(flaschentype)
include(FetchContent)

#FetchContent_Declare(
#    freetype
#    URL https://gitlab.freedesktop.org/freetype/freetype/-/archive/VER-2-13-2/freetype-VER-2-13-2.tar.gz
#)
#FetchContent_MakeAvailable(freetype)
if((NOT TYPST_EXECUTABLE) AND (NOT DISABLE_TYPST))
    set(TYPST_EXECUTABLE "typst-nightly")
endif()
set(shell_command "echo abc | ${TYPST_EXECUTABLE} c - -f svg -")
execute_process(COMMAND "sh" "-c" "${shell_command}" RESULT_VARIABLE resvar OUTPUT_FILE "/dev/null" ERROR_FILE "/dev/null")
if(${resvar} EQUAL 0)
    set(ENABLE_TYPST ON)
    FetchContent_Declare(
        lunasvg
        URL https://github.com/sammycage/lunasvg/archive/refs/tags/v2.3.9.tar.gz
    )
    FetchContent_MakeAvailable(lunasvg)
endif()
set(ENABLE_TYPST OFF)
configure_file(fltcfg.h.in fltcfg.h)
find_package(Freetype REQUIRED)
add_library(flaschentype STATIC "flaschentype.cpp")
target_include_directories(flaschentype PUBLIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(flaschentype PUBLIC Freetype::Freetype lunasvg)