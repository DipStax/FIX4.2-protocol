file(GLOB_RECURSE SRC_Shared_PgSQL
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_PgSQL STATIC
    ${SRC_Shared_PgSQL}
)

FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.10.1
)

# Disable tests/examples
set(LIBPQXX_BUILD_TESTS OFF CACHE BOOL "")
set(LIBPQXX_BUILD_EXAMPLES OFF CACHE BOOL "")

FetchContent_MakeAvailable(libpqxx)

target_link_libraries(Shared_PgSQL PUBLIC
    pqxx
)


target_include_directories(Shared_PgSQL PUBLIC include)