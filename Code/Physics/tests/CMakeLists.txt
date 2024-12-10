include_guard()

include("${PROJECT_SOURCE_DIR}/cmake/Utils.cmake")

add_executable(ge_engine_physics_tests
    "Main.cpp"

    "Collisions/TestAABB.cpp"
)
target_link_libraries(ge_engine_physics_tests
    INTERFACE
        GE::RootConfig
    PRIVATE
        GE::Engine::UnitTestFramework
        GE::Engine::Physics
)
ge_copyLibrariesOnPostBuild(ge_engine_physics_tests GE::Engine::Core GE::Engine::Physics)