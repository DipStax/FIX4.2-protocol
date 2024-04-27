
# Setup GTest
include(FetchContent)

set(GTEST_VERSION 1.14.0)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}/gtest
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/gtest
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE SRC_TESTS
    test/**.cpp
)

add_executable(MarketTest
    ${SRC_TESTS}
)
target_include_directories(MarketTest PRIVATE include)
target_include_directories(MarketTest PRIVATE test)
target_link_libraries(MarketTest LibCommon GTest::gtest_main)
target_compile_options(MarketTest PRIVATE -Wno-protected -Wno-private)

include(GoogleTest)
gtest_discover_tests(MarketTest)