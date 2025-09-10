# Server

file(GLOB_RECURSE SRC_Fix_Server
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_executable(Fix_Server
    ${SRC_Fix_Server}
)

add_custom_command(
    TARGET Fix_Server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/config.json
            $<TARGET_FILE_DIR:Fix_Server>/config.json
)

target_link_libraries(Fix_Server PRIVATE
    Shared_Configuration
    Shared_ProcessUnit
    Shared_Message
    Shared_Core
    Shared_Thread
    Shared_Log
    Shared_Network
)

target_include_directories(Fix_Server PRIVATE include)

target_compile_definitions(Fix_Server PRIVATE PROVIDER_NAME="MyMarket")           # Server name (used for TargetCompId from client)
target_compile_definitions(Fix_Server PRIVATE MARKET_NAME="GOLD","EURO","USD")    # Name of availalbe market