file(GLOB_RECURSE SRC_Shared_Network
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_Network STATIC
    ${SRC_Shared_Network}
)

target_compile_definitions(Shared_Network INTERFACE MAX_EVENT_EPOLL=30)
target_compile_definitions(Shared_Network INTERFACE MAX_SOCKET=30)

target_include_directories(Shared_Network PUBLIC include)

target_link_libraries(Shared_Network
    Shared_Utils
)

file(GLOB_RECURSE SRC_Shared_Network_Test
    tests/**.cpp
)

add_executable(Shared_Network_Test
    ${SRC_Shared_Network_Test}
)

target_link_libraries(Shared_Network_Test Shared_Network GTest::gtest_main)

gtest_discover_tests(Shared_Network_Test)
