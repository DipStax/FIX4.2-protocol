file(GLOB_RECURSE SRC_Shared_Log
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_Log STATIC
    ${SRC_Shared_Log}
)
# target_compile_options(COM_Log PRIVATE -Wno-builtin-declaration-mismatch)

target_include_directories(Shared_Log PUBLIC include)

target_link_libraries(Shared_Log
    Shared_Thread
)