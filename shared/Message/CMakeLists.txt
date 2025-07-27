
###################################
#   Message
###################################

file(GLOB_RECURSE SRC_Shared_Message
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_Message STATIC
    ${SRC_Shared_Message}
)

##### SOULD BE REMOVE
target_compile_definitions(Shared_Message PUBLIC MARKET_NAME="GOLD","EURO","USD")    # Server name (used for TargetCompId from client)

target_include_directories(Shared_Message PUBLIC include)

target_link_libraries(Shared_Message
    Shared_Container
)