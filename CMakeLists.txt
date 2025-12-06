cmake_minimum_required(VERSION 3.28)

project(safe-c)

add_library(safe-c STATIC)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
target_sources(safe-c PRIVATE ${SRC})

target_include_directories(safe-c PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
