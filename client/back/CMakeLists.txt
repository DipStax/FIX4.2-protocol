file(GLOB_RECURSE SRC_Fix_Client_Back
    include/**.hpp
    include/**.inl
    src/ProcessUnit/Network/TcpInput.cpp
    src/ProcessUnit/Network/TcpOutput.cpp
    src/ProcessUnit/data/Global.cpp
    src/Signal.cpp
    src/User.cpp
    src/Config.cpp
    src/Core.cpp
    src/FrontManager.cpp
    src/InitiatorManager.cpp
    src/main.cpp
)

add_executable(Fix_Client_Back
    ${SRC_Fix_Client_Back}
)

add_custom_command(
    TARGET Fix_Client_Back POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -DSRC=${CMAKE_CURRENT_SOURCE_DIR}/config.json
        -DDEST=$<TARGET_FILE_DIR:Fix_Client_Back>/config.json
        -DDST_DIR=$<TARGET_FILE_DIR:Fix_Client_Back>
        -P ${CMAKE_SOURCE_DIR}/cmake/copy_config.cmake
)

target_link_libraries(Fix_Client_Back PRIVATE
    Shared_Configuration
    Shared_Message
    Shared_Message_v2
    Shared_Thread
    Shared_Network
    Shared_Core
    Shared_ProcessUnit
    Client_Shared_IPC
)

target_include_directories(Fix_Client_Back PUBLIC include)