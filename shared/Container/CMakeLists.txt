
file(GLOB_RECURSE SRC_Shared_Container
    include/**.hpp
    include/**.inl
)

add_library(Shared_Container INTERFACE
    ${SRC_Shared_Container}
)

target_include_directories(Shared_Container INTERFACE include)

target_link_libraries(Shared_Container INTERFACE
    Shared_Utils
)
