file(GLOB_RECURSE SRC_Shared_Core
    include/**.hpp
    include/**.inl
    src/**.cpp
)
message (STATUS "test " ${SRC_Shared_Core})

add_library(Shared_Core STATIC
    ${SRC_Shared_Core}
)

target_include_directories(Shared_Core PUBLIC include)