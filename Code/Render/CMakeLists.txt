include_guard()

add_library(ge_engine_render
    "src/Shaders.cpp"
    "src/LogRender.cpp"
    "src/RenderingSystem.cpp"
    "src/Renderable.cpp"
)
add_library(GE::Engine::Render ALIAS ge_engine_render)
target_include_directories(ge_engine_render PUBLIC "include/")
target_compile_definitions(ge_engine_render PRIVATE RENDER_API_EXPORTS)
target_link_libraries(ge_engine_render
    INTERFACE
        GE::RootConfig
    PUBLIC
        GE::Engine::Core
    PRIVATE
        GE::ThirdParty::glad
)