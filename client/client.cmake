# Client


file(GLOB_RECURSE SRC_CLIENT
    client/include/**.hpp
    client/include/**.inl
    client/src/**.cpp
)

add_executable(MarketClient
    ${SRC_CLIENT}
)

target_link_libraries(MarketClient PRIVATE LibCommon)

target_include_directories(MarketClient PRIVATE common/include)
target_include_directories(MarketClient PRIVATE client/include)

target_compile_definitions(MarketClient PRIVATE PROVIDER_NAME="MyMarket")
target_compile_definitions(MarketClient PRIVATE MARKET_NAME="GOLD","EURO","USD")