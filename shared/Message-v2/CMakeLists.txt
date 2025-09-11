file(GLOB_RECURSE SRC_Shared_Message_v2
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_Message_v2 STATIC
    ${SRC_Shared_Message_v2}
)

target_link_libraries(Shared_Message_v2
    fix_message
)

target_include_directories(Shared_Message_v2 PUBLIC include)