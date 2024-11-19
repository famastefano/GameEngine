cmake_minimum_required(VERSION 3.31)

project(GameEngine VERSION 0.1.0 LANGUAGES CXX)

if(${GE_BUILD_ENABLE_MONOLITHIC})
    set(GE_BUILD_ADD_LIBRARY_TYPE STATIC)
else()
    set(GE_BUILD_ADD_LIBRARY_TYPE SHARED)
endif()

add_library(ge_root_config_target INTERFACE)
add_library(GE::RootConfig ALIAS ge_root_config_target)
target_compile_definitions(ge_root_config_target INTERFACE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    GE_BUILD_CONFIG_${GE_BUILD_CONFIG}
    GE_BUILD_ENABLE_MONOLITHIC=$<BOOL:${GE_BUILD_ENABLE_MONOLITHIC}>
    GE_BUILD_EDITOR=$<BOOL:${GE_BUILD_EDITOR}>
)
target_compile_features(ge_root_config_target INTERFACE cxx_std_20)

if(${GE_BUILD_CONFIG} STREQUAL "DEBUG")
    target_compile_options(ge_root_config_target INTERFACE /fsanitize=address)
    target_link_options(ge_root_config_target INTERFACE /fsanitize=address)
endif()

# 3rd-party libs - Before any other project as those are self-contained and possibly consumed by other projects
add_subdirectory("Code/ThirdParty/xxhash")
add_subdirectory("Code/ThirdParty/RapidJson")

# Standalone unit test framework used by any other library
add_subdirectory("Code/UnitTestFramework")

# Engine modules
add_subdirectory("Code/Core")
add_subdirectory("Code/Math")
add_subdirectory("Code/Input")

# Runners
add_subdirectory("Code/Application")