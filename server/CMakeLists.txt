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
    COMMAND ${CMAKE_COMMAND}
        -DSRC=${CMAKE_CURRENT_SOURCE_DIR}/config.json
        -DDEST=$<TARGET_FILE_DIR:Fix_Server>/config.json
        -DDST_DIR=$<TARGET_FILE_DIR:Fix_Server>
        -P ${CMAKE_SOURCE_DIR}/cmake/copy_config.cmake
)

target_link_libraries(Fix_Server PRIVATE
    Shared_Message_v2
    Shared_Configuration
    Shared_ProcessUnit
    Shared_Core
    Shared_Thread
    Shared_Log
    Shared_Network
)

target_include_directories(Fix_Server PRIVATE include)