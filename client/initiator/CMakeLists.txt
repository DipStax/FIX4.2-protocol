file(GLOB_RECURSE SRC_Fix_Client_Initiator
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_executable(Fix_Client_Initiator
    ${SRC_Fix_Client_Initiator}
)

target_link_libraries(Fix_Client_Initiator PRIVATE
    Client_Shared_IPC
    Shared_Core
    Shared_Network
    Shared_ProcessUnit
    nlohmann_json::nlohmann_json
)

target_include_directories(Fix_Client_Initiator PUBLIC include)
