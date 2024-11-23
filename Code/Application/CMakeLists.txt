include_guard()

include("${PROJECT_SOURCE_DIR}/cmake/Utils.cmake")

add_executable(ge_runner_win32
    "src/LogApplication.cpp"
)
if(WIN32)
    set_target_properties(ge_runner_win32
        PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
    target_sources(ge_runner_win32 PRIVATE
        "src/Win32Application.cpp"
    )
else()
    message(FATAL_ERROR "Only a Win32 Application is supported.")
endif()

target_include_directories(ge_runner_win32 PRIVATE "include/")
target_link_libraries(ge_runner_win32
    INTERFACE GE::RootConfig
    PRIVATE GE::Engine::Core
    PRIVATE GE::Engine::Input
    PRIVATE GE::Engine::Render
    PRIVATE GE::ThirdParty::glad
)

ge_copyLibrariesOnPostBuild(ge_runner_win32 GE::Engine::Core GE::Engine::Input)