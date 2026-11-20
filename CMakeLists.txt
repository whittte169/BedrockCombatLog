cmake_minimum_required(VERSION 3.15)

project(BedrockCombatLog VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENDSTONE_API_VERSION "0.11" CACHE STRING "Endstone API version")
set(BOOST_INCLUDE_LIBRARIES unordered)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
        endstone
        GIT_REPOSITORY https://github.com/EndstoneMC/endstone.git
        GIT_TAG v${ENDSTONE_API_VERSION}
)

FetchContent_Declare(
        Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.87.0
        GIT_SUBMODULES "libs/unordered;libs/config;libs/core;libs/assert;libs/throw_exception;libs/mp11;libs/predef;libs/static_assert;libs/type_traits;libs/container_hash;libs/describe;tools/cmake"
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(endstone Boost)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(include/version.hpp.in include/version.hpp)

endstone_add_plugin(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include)
