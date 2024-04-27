# Server

file(GLOB_RECURSE SRC_SERVER
    server/include/**.hpp
    server/include/**.inl
    server/src/**.cpp
)

add_executable(Server
    ${SRC_SERVER}
)

target_link_libraries(Server PRIVATE LibCommon)

target_include_directories(Server PRIVATE common/include)
target_include_directories(Server PRIVATE server/include)

target_compile_definitions(Server PRIVATE TS_SIZE_OB=1)                       # Thread Pool size from Order Book
target_compile_definitions(Server PRIVATE TS_SIZE_ON=1)                       # Thread Pool size from Output network
target_compile_definitions(Server PRIVATE NET_RECV_SIZE=4096)                 # Package max read size
target_compile_definitions(Server PRIVATE PROVIDER_NAME="MyMarket")           # Server name (used for TargetCompId from client)
target_compile_definitions(Server PRIVATE MARKET_NAME="GOLD","EURO","USD")    # Name of availalbe market