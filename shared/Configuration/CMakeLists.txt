file(GLOB_RECURSE SRC_Shared_Configuration
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_library(Shared_Configuration INTERFACE
    ${SRC_Shared_Configuration}
)


FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(json)

target_link_libraries(Shared_Configuration INTERFACE
    nlohmann_json::nlohmann_json
)

target_include_directories(Shared_Configuration INTERFACE include)
