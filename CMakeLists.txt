cmake_minimum_required(VERSION 3.31)

project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

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
    _CRT_SECURE_NO_WARNINGS
    GE_BUILD_CONFIG_${GE_BUILD_CONFIG}
    GE_BUILD_ENABLE_MONOLITHIC=$<BOOL:${GE_BUILD_ENABLE_MONOLITHIC}>
    GE_BUILD_EDITOR=$<BOOL:${GE_BUILD_EDITOR}>
    GE_PROFILING_ENABLED=$<BOOL:${GE_PROFILING_ENABLED}>
)
target_compile_options(ge_root_config_target INTERFACE /W4 /EHa-)

if(${GE_BUILD_CONFIG} STREQUAL "DEBUG")
    target_compile_options(ge_root_config_target INTERFACE /fsanitize=address)
    target_link_options(ge_root_config_target INTERFACE /fsanitize=address)
endif()

# Core - This can be used by any project, even 3rd-party ones
add_subdirectory("Code/Core")

# 3rd-party libs - Before any other project as those are self-contained and possibly consumed by other projects
add_subdirectory("Code/ThirdParty/tracy") # Tracy 1st, as we might add it to other libs

add_subdirectory("Code/ThirdParty/RapidJson")
add_subdirectory("Code/ThirdParty/glad")
add_subdirectory("Code/ThirdParty/entt")
add_subdirectory("Code/ThirdParty/box2d")

# Standalone unit test framework used by any other library made by me
add_subdirectory("Code/UnitTestFramework")

add_subdirectory("Code/Shared")
add_subdirectory("Code/Math")
add_subdirectory("Code/Input")
add_subdirectory("Code/Render")

# Runners
add_subdirectory("Code/Application")