file(GLOB_RECURSE SRC_Fix_Client_Back
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_executable(Fix_Client_Back
    ${SRC_Fix_Client_Back}
)

target_compile_definitions(Fix_Client_Back PUBLIC PROVIDER_NAME="MyMarket")

target_link_libraries(Fix_Client_Back PRIVATE
    Shared_Message
    Shared_Thread
    Shared_Network
    Shared_Core
    Shared_ProcessUnit
    Client_Shared_IPC
)

target_include_directories(Fix_Client_Back PUBLIC include)