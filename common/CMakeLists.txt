# Common library

project(Common_Library)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

set(CR ${CMAKE_CURRENT_LIST_DIR})

include_directories("${CR}/include")

add_compile_options(-Wall -Wextra -g3)

###################################
#   Utils
###################################

file(GLOB SRC_Utils
    "${CR}/include/Common/*.hpp"
    "${CR}/include/Common/*.inl"
    "${CR}/src/*.cpp"
)

add_library(COM_Utils STATIC
    ${SRC_Utils}
)

message(STATUS "Utils sources: \"${CR}/include/Common/*.hpp\"")

###################################
#   Core
###################################

file(GLOB_RECURSE SRC_Core
    "${CR}/include/Common/Core/**.hpp"
    "${CR}/include/Common/Core/**.inl"
    "${CR}/src/Core/**.cpp"
)

add_library(COM_Core STATIC
    ${SRC_Core}
)

###################################
#   Container
###################################

file(GLOB_RECURSE SRC_Container
    "${CR}/include/Common/Container/**.hpp"
    "${CR}/Common/Container/**.inl"
)

add_library(COM_Container INTERFACE
    ${SRC_Container}
)

message(STATUS "Container sources: ${SRC_Container}")

###################################
#   Log
###################################

file(GLOB_RECURSE SRC_Log
    "${CR}/include/Common/Log/**.hpp"
    "${CR}/include/Common/Log/**.inl"
    "${CR}/src/Log/**.cpp"
)

add_library(COM_Log STATIC
    ${SRC_Log}
)
target_compile_options(COM_Log PRIVATE -Wno-builtin-declaration-mismatch)
###################################
#   Message
###################################

file(GLOB_RECURSE SRC_Message
    "${CR}/include/Common/Message/**.hpp"
    "${CR}/include/Common/Message/**.inl"
    "${CR}/src/Message/**.cpp"
)

add_library(COM_Message STATIC
    ${SRC_Message}
)

##### SOULD BE REMOVE
target_link_libraries(COM_Message
    PRIVATE
        COM_Utils
)
##### SOULD BE REMOVE
target_compile_definitions(COM_Message PUBLIC MARKET_NAME="GOLD","EURO","USD")    # Server name (used for TargetCompId from client)

###################################
#   Network
###################################

file(GLOB_RECURSE SRC_Network
    "${CR}/include/Common/Network/**.hpp"
    "${CR}/include/Common/Network/**.inl"
    "${CR}/src/Network/**.cpp"
)

add_library(COM_Network STATIC
    ${SRC_Network}
)
target_compile_definitions(COM_Network INTERFACE MAX_EVENT_EPOLL=30)            # Maximum of event handle by epoll at once
target_compile_definitions(COM_Network INTERFACE MAX_SOCKET=30)                 # Maximum of socket handle by epoll/socket at once
target_compile_definitions(COM_Network INTERFACE MAX_RECV_SIZE=2048)            # Maximum of data received bu a socket

###################################
#   Thread
###################################

file(GLOB_RECURSE SRC_Thread
    "${CR}/include/Common/Thread/**.hpp"
    "${CR}/include/Common/Thread/**.inl"
)

add_library(COM_Thread INTERFACE
    ${SRC_Thread}
)
target_compile_definitions(COM_Thread INTERFACE N_THREAD_TP=1)                 # Default number of thread in a thread pool

###################################
#   LibCommon
###################################

add_library(LibCommon INTERFACE)

target_link_libraries(LibCommon
    INTERFACE
        COM_Container
        COM_Utils
        COM_Core
        COM_Log
        COM_Message
        COM_Network
        COM_Thread
)