cmake_minimum_required(VERSION 3.22.1)

project(FIX_Protocol)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

add_compile_options(-Wall -Wextra -Wpedantic -g3)

option(CommonBuild "Build the Common library" FALSE)
option(ServerBuild "Build the Server library" FALSE)
option(ClientBuild "Build the Client library" FALSE)

if (ServerBuild)
    set(CommonBuild TRUE)
    add_subdirectory(server)
endif()

if (ClientBuild)
    set(CommonBuild TRUE)
    add_subdirectory(client)
endif()

if (CommonBuild)
    add_subdirectory(shared)
endif()