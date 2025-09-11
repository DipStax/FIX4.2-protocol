
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

file(GLOB_RECURSE SRC_Fix_Client_GUI
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_executable(Fix_Client_GUI
    ${SRC_Fix_Client_GUI}
)

add_custom_command(
    TARGET Fix_Client_GUI POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -DSRC=${CMAKE_CURRENT_SOURCE_DIR}/config.json
        -DDEST=$<TARGET_FILE_DIR:Fix_Client_GUI>/config.json
        -DDST_DIR=$<TARGET_FILE_DIR:Fix_Client_GUI>
        -P ${CMAKE_SOURCE_DIR}/cmake/copy_config.cmake
)

target_link_libraries(Fix_Client_GUI PRIVATE
    Client_Shared_IPC
    Shared_Configuration
    Shared_ProcessUnit
    Shared_Network
    Shared_Core
    Qt6::Core
    Qt6::Widgets
)

target_compile_definitions(Fix_Client_GUI PUBLIC MARKET_NAME="GOLD","EURO","USD")    # Server name (used for TargetCompId from client)

target_include_directories(Fix_Client_GUI PUBLIC include)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
