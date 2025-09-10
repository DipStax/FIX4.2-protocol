# Common library

project(FIX_Shared)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

include(FetchContent)

add_compile_options(-Wall -Wextra -g3)

enable_testing()

set(GTEST_VERSION 1.17.0)
FetchContent_Declare(
  googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v${GTEST_VERSION}
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Thread)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Configuration)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Container)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Core)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Log)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ProcessUnit)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Message)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Network)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Utils)

