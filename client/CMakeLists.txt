cmake_minimum_required(VERSION 3.22.1)

project(FIX_Client)

include(FetchContent)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

set(CRLD ${CMAKE_CURRENT_LIST_DIR})

add_compile_options(-Wall -Wextra -g3)

include_directories(common/include/)

include("${CRLD}/Common/CMakeLists.txt")

###################################
#   GUI
###################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

file(GLOB_RECURSE SRC_FIXClient_GUI
    "${CRLD}/GUI/include/**.hpp"
    "${CRLD}/GUI/include/**.inl"
    "${CRLD}/GUI/src/**.cpp"
)

add_executable(FIX_Client_GUI
    ${SRC_FIXClient_GUI}
)

target_link_libraries(FIX_Client_GUI PRIVATE LibCommon CLIENT_COM_IPC)
target_link_libraries(FIX_Client_GUI PRIVATE Qt6::Core Qt6::Widgets)

target_include_directories(FIX_Client_GUI PUBLIC "${CRLD}/GUI/include")

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

###################################
#   Backend
###################################

file(GLOB_RECURSE SRC_FIXClient_Back
    "${CRLD}/Back/include/**.hpp"
    "${CRLD}/Back/include/**.inl"
    "${CRLD}/Back/src/**.cpp"
)

add_executable(FIX_Client_Back
    ${SRC_FIXClient_Back}
)

target_include_directories(FIX_Client_Back PUBLIC "${CRLD}/Back/include")

target_compile_definitions(FIX_Client_Back PUBLIC PROVIDER_NAME="MyMarket")

target_link_libraries(FIX_Client_Back PRIVATE LibCommon CLIENT_COM_IPC)