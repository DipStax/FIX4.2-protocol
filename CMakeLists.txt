cmake_minimum_required(VERSION 3.22.1)

project(Epifinance)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

add_compile_options(-Wall -Wextra -g3)

option(CommonBuild "Build the Common library" FALSE)
option(ServerBuil "Build the Server library" FALSE)
option(ClientBuild "Build the Client library" FALSE)

if (ServerBuild)
    include(server/server.cmake)

    if (CommonBuild)
        set(CommonBuild TRUE)
    endif()
endif()

if (ClientBuild)
    include(common/common.cmake)
    include(client/client.cmake)

    if (CommonBuild)
        set(CommonBuild TRUE)
    endif()
endif()

if (CommonBuild)
    include(common/common.cmake)
endif()