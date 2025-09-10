file(GLOB_RECURSE SRC_Shared_Thread
    include/**.hpp
    include/**.inl
)

add_library(Shared_Thread INTERFACE
    ${SRC_Shared_Thread}
)

target_compile_definitions(Shared_Thread INTERFACE N_THREAD_TP=1)

target_include_directories(Shared_Thread INTERFACE include)
