cmake_minimum_required(VERSION 3.22.1)

project(Epifinance)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

add_compile_options(-Wall -Wextra -g3)

option(CommonBuild "Build the Common library" FALSE)
option(ServerBuild "Build the Server library" FALSE)
option(ClientBuild "Build the Client library" FALSE)

if (ServerBuild)
    set(CommonBuild TRUE)
    include(server/server.cmake)
endif()

if (ClientBuild)
    set(CommonBuild TRUE)
    include(client/client.cmake)
endif()

if (CommonBuild)
    include(common/CMakeLists.txt)
endif()