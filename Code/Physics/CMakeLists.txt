include_guard()

add_library(ge_engine_physics
    "src/AABB.cpp"
    "src/LogPhysics.cpp"
)

add_library(GE::Engine::Physics ALIAS ge_engine_physics)
target_include_directories(ge_engine_physics PUBLIC "include/")
target_compile_definitions(ge_engine_physics PRIVATE PHYSICS_API_EXPORTS)
target_link_libraries(ge_engine_physics
    INTERFACE GE::RootConfig
    PUBLIC GE::Engine::Core
    PUBLIC GE::Engine::Math
)

if(GE_BUILD_ENABLE_TESTS)
    add_subdirectory("tests")
endif()