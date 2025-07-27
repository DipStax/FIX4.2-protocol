file(GLOB_RECURSE SRC_Shared_ProcessUnit
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_ProcessUnit STATIC
    ${SRC_Shared_ProcessUnit}
)
