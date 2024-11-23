include_guard()

add_library(ge_engine_render ${GE_BUILD_ADD_LIBRARY_TYPE}
    "src/Shaders.cpp"

    "src/LogRender.cpp"
)
add_library(GE::Engine::Render ALIAS ge_engine_render)
target_include_directories(ge_engine_render PUBLIC "include/")
target_compile_definitions(ge_engine_render PRIVATE RENDER_API_EXPORTS)
target_link_libraries(ge_engine_render
    INTERFACE ge_root_config_target
    PUBLIC GE::Engine::Core
    PRIVATE GE::ThirdParty::glad
)