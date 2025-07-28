file(GLOB_RECURSE SRC_Client_Shared_IPC
    include/**.hpp
    include/**.inl
    src/*.cpp
)

add_library(Client_Shared_IPC STATIC
    ${SRC_Client_Shared_IPC}
)

target_link_libraries(Client_Shared_IPC PRIVATE
    Shared_Network
    Shared_Core
    Shared_ProcessUnit
)

target_include_directories(Client_Shared_IPC PUBLIC include)