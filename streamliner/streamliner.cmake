# Stream liner

include(FetchContent)

file(GLOB_RECURSE SRC_STREAML
    streamliner/include/**.hpp
    streamliner/include/**.inl
    streamliner/src/**.cpp
)

add_executable(Streamliner
    ${SRC_STREAML}
)

target_include_directories(Streamliner PRIVATE streamliner/include)

set(NJSON_VERSION 3.11.3)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v${NJSON_VERSION}/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(Streamliner PRIVATE nlohmann_json::nlohmann_json)