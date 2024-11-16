include_guard()

include("${PROJECT_SOURCE_DIR}/cmake/Utils.cmake")

add_executable(ge_engine_core_tests
    "Main.cpp"

    "src/Allocator/TestsGlobalAllocator.cpp"

    "src/Container/TestSpan.cpp"
    "src/Container/TestStringView.cpp"
    "src/Container/TestVector.cpp"
)
target_link_libraries(ge_engine_core_tests
    INTERFACE
        GE::RootConfig
    PRIVATE
        GE::Engine::UnitTestFramework
        GE::Engine::Core
)
ge_copyLibrariesOnPostBuild(ge_engine_core_tests GE::Engine::Core)