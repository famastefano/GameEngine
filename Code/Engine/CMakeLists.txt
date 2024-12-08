include_guard()

add_library(ge_engine_game_engine ${GE_BUILD_ADD_LIBRARY_TYPE}
    "src/Environment.cpp"
    "src/EventBase.cpp"
    "src/GameEngine.cpp"
)
add_library(GE::Engine::Engine ALIAS ge_engine_game_engine)
target_include_directories(ge_engine_game_engine PUBLIC "include/")
target_compile_definitions(ge_engine_game_engine PRIVATE ENGINE_API_EXPORTS)
target_link_libraries(ge_engine_game_engine
    INTERFACE
        ge_root_config_target
    PUBLIC
        GE::Engine::Core
)