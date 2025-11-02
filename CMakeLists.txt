cmake_minimum_required(VERSION 3.23)

project(GraphficsProject C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_library(glad)

target_sources(glad
    PRIVATE
    src/glad.c

  PUBLIC
    FILE_SET glad_headers
    TYPE HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h
)

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a"
)

add_executable(Graphfics)

target_sources(Graphfics
  PRIVATE
    src/main.cpp
    src/shaders.cpp
    src/stb_image.cpp
  
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      include
    FILES
      include/shaders.hpp
      include/stb_image.h
)

find_library(OPENGL_FRAMEWORK OpenGL)
find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)

target_link_libraries(Graphfics
  PRIVATE
    ${OPENGL_FRAMEWORK}
    ${COCOA_FRAMEWORK}
    ${IOKIT_FRAMEWORK}
    glad
    glfw
)
