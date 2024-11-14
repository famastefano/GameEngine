include_guard()

add_library(ge_engine_unit_test_framework STATIC
    "include/UnitTest/UnitTest.h"
    "src/UnitTest.cpp"
)
add_library(GE::Engine::UnitTestFramework ALIAS ge_engine_unit_test_framework)
target_link_libraries(ge_engine_unit_test_framework INTERFACE GE::RootConfig PRIVATE GE::Engine::Core)
target_include_directories(ge_engine_unit_test_framework PUBLIC "include/")