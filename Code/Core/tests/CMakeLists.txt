include_guard()

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
add_custom_command(TARGET ge_engine_core_tests
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ge_engine_core> $<TARGET_FILE_DIR:ge_engine_core_tests>
)