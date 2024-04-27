# Common library

file(GLOB_RECURSE SRC_COMMON
    common/include/**.hpp
    common/include/**.inl
    common/src/**.cpp
)

add_library(LibCommon SHARED
    ${SRC_COMMON}
)

target_include_directories(LibCommon PRIVATE common/include)

add_compile_definitions(MAX_EVENT_EPOLL=30)            # Maximum of event handle by epoll at once
add_compile_definitions(MAX_SOCKET=30)                 # Maximum of socket handle by epoll/socket at once
add_compile_definitions(N_THREAD_TP=1)                 # Default number of thread in a thread pool
add_compile_definitions(LOG_DFT_FILE="./default.log")  # Default log file
add_compile_definitions(MAX_RECV_SIZE=2048)            # Maximum of data received bu a socket
add_compile_definitions(MARKET_NAME="GOLD","EURO","USD")    # Server name (used for TargetCompId from client)
add_compile_definitions(NOTIF_UPDATE_TO=20)           # Time between each incremental refresh
add_compile_definitions(NOTIF_REFRESH_TO=20)           # Time between each full refresh
