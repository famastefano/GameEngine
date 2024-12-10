include_guard()

add_library(ge_engine_core
    "src/StaticInit.cpp"

    "src/Allocator/GlobalAllocator.cpp"

    "src/Hash/Hash.cpp"
    "src/Hash/xxhash.c"

    "src/Logging/Logging.cpp"
    "src/Logging/CoreLogging.cpp"

    "src/Platform/Platform.cpp"

    "Core.natvis"
)
add_library(GE::Engine::Core ALIAS ge_engine_core)
target_compile_definitions(ge_engine_core PRIVATE CORE_API_EXPORTS)
target_include_directories(ge_engine_core PUBLIC "include/")
target_link_libraries(ge_engine_core INTERFACE GE::RootConfig)

if(GE_BUILD_ENABLE_TESTS)
    add_subdirectory("tests")
endif()