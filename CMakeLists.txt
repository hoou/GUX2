cmake_minimum_required(VERSION 3.7)
project(GUX2)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

set(SOURCE_FILES main.c)
add_executable(GUX2 ${SOURCE_FILES})

target_link_libraries(GUX2 ${GTK3_LIBRARIES})