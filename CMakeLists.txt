cmake_minimum_required(VERSION 3.30)

cmake_policy(SET CMP0091 NEW)

# CMAKE - Build Debugging - Various flags to help debugging MSVC shenanigans
#set(VERBOSE ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /verbose /linkrepro:C:\\linkrepro")
#set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} /verbose")
#set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /verbose")

project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

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

set(GE_PREV_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

if(GE_PROFILING_ENABLED)
    # 3rd-party libs - Before any other project as those are self-contained and possibly consumed by other projects
    add_subdirectory("Code/ThirdParty/tracy") # Tracy 1st, as we might add it to other libs
endif()

add_subdirectory("Code/ThirdParty/RapidJson")
add_subdirectory("Code/ThirdParty/glad")

# Standalone unit test framework used by any other library made by me
if(GE_BUILD_ENABLE_TESTS)
    add_subdirectory("Code/UnitTestFramework")
endif()

add_subdirectory("Code/Math")
add_subdirectory("Code/Input")
#add_subdirectory("Code/Render")
add_subdirectory("Code/Physics")

# Needs to be last as we link all the libraries into the Engine
add_subdirectory("Code/Engine")

# Runners
add_subdirectory("Code/Application")

# CMAKE - Build Debugging - Prints all variables
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()