cmake_minimum_required(VERSION 3.22.1)

project(FIX_Client)

include(FetchContent)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

set(CRLD ${CMAKE_CURRENT_LIST_DIR})

add_compile_options(-Wall -Wextra -g3)

###################################
#   Backend
###################################

add_subdirectory(back)
add_subdirectory(GUI)
add_subdirectory(shared)