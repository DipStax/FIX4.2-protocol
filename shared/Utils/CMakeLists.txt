file(GLOB_RECURSE SRC_Shared_Utils
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_Utils STATIC
    ${SRC_Shared_Utils}
)

target_include_directories(Shared_Utils PUBLIC include)
