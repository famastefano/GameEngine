include_guard()

# The Engine library will be linked by the runner executables, like ge_runner_win32.
# This is necessary as with a monolithic build, where everything is statically linked,
# MSVC link.exe will find MSVCRT.lib before ge_engine_core.lib, where we override new/delete to use the GlobalAllocator.
# This issues a linker error, where multiple symbols are defined, thus making it impossible to build.
# This lookup behaviour happens because the linker finds wWinMain (or whatever inside the .exe),
# starts loading stuff, finds MSVCRT.lib, loads its symbols, then linka against the monolithic library, and fails.
# https://developercommunity.visualstudio.com/t/visual-studio-2017-msvcrtlib-link-error/534202
add_library(ge_engine_game_engine SHARED
        "src/Environment.cpp"
        "src/GameEngine.cpp"

        "src/LogCategories.cpp"
        "src/SubSystem.cpp"
        "src/SubSystems/Renderer/RendererSubSystem.cpp"
        "src/SubSystems/Input/InputSubSystem.cpp"

        "src/EventsMetadata.cpp"
)
add_library(GE::Engine::Engine ALIAS ge_engine_game_engine)

if (WIN32)
    target_sources(ge_engine_game_engine PRIVATE "src/SubSystems/Input/Base/Win32Translator.cpp")
endif ()

target_include_directories(ge_engine_game_engine PUBLIC "include/" "src/include")
target_compile_definitions(ge_engine_game_engine PRIVATE ENGINE_API_EXPORTS)
target_link_libraries(ge_engine_game_engine
    INTERFACE
        ge_root_config_target
    PUBLIC
        GE::Engine::Core
        PRIVATE
        GE::ThirdParty::glad
)