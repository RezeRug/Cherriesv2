cmake_minimum_required(VERSION 3.20)
project(cherries LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  Crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG v1.2.1
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Crow)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(cherries src/main.cpp)
target_link_libraries(cherries PRIVATE Crow::Crow nlohmann_json::nlohmann_json)
target_compile_definitions(cherries PRIVATE
  CHERRIES_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)
if (WIN32)
  target_link_libraries(cherries PRIVATE ws2_32)
endif()
