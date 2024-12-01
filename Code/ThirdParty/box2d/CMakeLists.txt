include_guard()

file(GLOB_RECURSE GE_3RDPARTY_BOX2D_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "include/*.h" "src/*.h" "src/*.c")
add_library(ge_3rdparty_box2d ${GE_BUILD_ADD_LIBRARY_TYPE}
    ${GE_3RDPARTY_BOX2D_SOURCES}
)
add_library(GE::ThirdParty:Box2D ALIAS ge_3rdparty_box2d)
target_include_directories(ge_3rdparty_box2d PUBLIC "include/")
target_link_libraries(ge_3rdparty_box2d INTERFACE ge_root_config_target)

# Box2D uses C17
set_target_properties(ge_3rdparty_box2d PROPERTIES
	C_STANDARD 17
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_sources(ge_3rdparty_box2d PRIVATE "src/box2d.natvis")
endif()

target_compile_definitions(ge_3rdparty_box2d PRIVATE BOX2D_ENABLE_EXPORT BOX2D_AVX2 BOX2D_ENABLE_SIMD)

# Atomics are still considered experimental in Visual Studio 17.8
target_compile_options(ge_3rdparty_box2d PRIVATE "/experimental:c11atomics")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(ge_3rdparty_box2d PRIVATE BOX2D_VALIDATE)
endif()

if (GE_PROFILING_ENABLED)
	target_compile_definitions(ge_3rdparty_box2d PRIVATE BOX2D_PROFILE)
	target_link_libraries(ge_3rdparty_box2d PRIVATE GE::ThirdParty::TracyClient)
endif()
