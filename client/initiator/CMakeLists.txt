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
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/config.json
            $<TARGET_FILE_DIR:Fix_Client_Initiator>/config.json
)

target_link_libraries(Fix_Client_Initiator PRIVATE
    Client_Shared_IPC
    Shared_Core
    Shared_Network
    Shared_ProcessUnit
    Shared_Configuration
)

target_include_directories(Fix_Client_Initiator PUBLIC include)
