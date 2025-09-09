cmake_minimum_required(VERSION 3.22.1)

project(FIX_Client)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

add_compile_options(-Wall -Wextra -g3)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(json)

add_subdirectory(back)
add_subdirectory(GUI)
add_subdirectory(initiator)
add_subdirectory(shared)