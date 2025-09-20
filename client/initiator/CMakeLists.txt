include(FetchContent)

file(GLOB_RECURSE SRC_Fix_Client_Initiator
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_executable(Fix_Client_Initiator
    ${SRC_Fix_Client_Initiator}
)

add_custom_command(
    TARGET Fix_Client_Initiator POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -DSRC=${CMAKE_CURRENT_SOURCE_DIR}/config.json
        -DDEST=$<TARGET_FILE_DIR:Fix_Client_Initiator>/config.json
        -DDST_DIR=$<TARGET_FILE_DIR:Fix_Client_Initiator>
        -P ${CMAKE_SOURCE_DIR}/cmake/copy_config.cmake
)



target_link_libraries(Fix_Client_Initiator PRIVATE
    Client_Shared_IPC
    Shared_Core
    Shared_Network
    Shared_ProcessUnit
    Shared_Configuration
    Shared_PgSQL
)

target_include_directories(Fix_Client_Initiator PUBLIC include)
