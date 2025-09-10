
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

target_link_libraries(Fix_Client_GUI PRIVATE
    Client_Shared_IPC
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
