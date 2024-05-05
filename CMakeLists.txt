cmake_minimum_required(VERSION 3.22.1)

project(Epifinance)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

add_compile_options(-Wall -Wextra -g3)

option(CommonBuild "Build the Common library" FALSE)
option(ServerBuild "Build the Server library" FALSE)
option(ClientBuild "Build the Client library" FALSE)
option(StreamLiner "Build the Streamline framework" FALSE)

if (ServerBuild)
    include(common/common.cmake)
    include(server/server.cmake)
endif()

if (ClientBuild)
    include(common/common.cmake)
    include(client/client.cmake)
endif()

if (CommonBuild)
    include(common/common.cmake)
endif()

if (StreamLiner)
    include(streamliner/streamliner.cmake)
endif()