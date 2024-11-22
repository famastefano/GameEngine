include_guard()

if(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
    set(GE_3RDPARTY_GLAG_DIR "dbg")
else()
    set(GE_3RDPARTY_GLAG_DIR "rel")
endif()

add_library(ge_3rdparty_glad STATIC
    "${GE_3RDPARTY_GLAG_DIR}/src/glad.c"
)
add_library(GE::ThirdParty::glad ALIAS ge_3rdparty_glad)
target_link_libraries(ge_3rdparty_glad INTERFACE ge_root_config_target)
target_include_directories(ge_3rdparty_glad PUBLIC "${GE_3RDPARTY_GLAG_DIR}/include")
set_target_properties(ge_3rdparty_glad PROPERTIES LINKER_LANGUAGE CXX)