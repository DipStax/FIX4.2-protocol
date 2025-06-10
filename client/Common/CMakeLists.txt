set(CR ${CMAKE_CURRENT_LIST_DIR})

include_directories("${CR}/include")

file(GLOB_RECURSE SRC_IPC
    "${CR}/include/Client/Common/ipc/**.hpp"
    "${CR}/include/Client/Common/ipc/**.inl"
    "${CR}/src/ipc/*.cpp"
)

add_library(CLIENT_COM_IPC STATIC
    ${SRC_IPC}
)

target_link_libraries(CLIENT_COM_IPC PRIVATE COM_Network)
target_include_directories(CLIENT_COM_IPC PRIVATE common/include/)
target_include_directories(CLIENT_COM_IPC PRIVATE "${CR}/include")