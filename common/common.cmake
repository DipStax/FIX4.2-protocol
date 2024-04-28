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

target_compile_definitions(LibCommon PUBLIC MAX_EVENT_EPOLL=30)            # Maximum of event handle by epoll at once
target_compile_definitions(LibCommon PUBLIC MAX_SOCKET=30)                 # Maximum of socket handle by epoll/socket at once
target_compile_definitions(LibCommon PUBLIC N_THREAD_TP=1)                 # Default number of thread in a thread pool
target_compile_definitions(LibCommon PUBLIC LOG_DFT_FILE="./default.log")  # Default log file
target_compile_definitions(LibCommon PUBLIC MAX_RECV_SIZE=2048)            # Maximum of data received bu a socket
target_compile_definitions(LibCommon PUBLIC MARKET_NAME="GOLD","EURO","USD")    # Server name (used for TargetCompId from client)
target_compile_definitions(LibCommon PUBLIC NOTIF_UPDATE_TO=20)            # Time between each incremental refresh
target_compile_definitions(LibCommon PUBLIC NOTIF_REFRESH_TO=20)           # Time between each full refresh
