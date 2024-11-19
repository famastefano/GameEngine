include_guard()

add_library(ge_engine_math STATIC
    "include/Math/Vector.h"
    "src/Vector.cpp"
)

add_library(GE::Engine::Math ALIAS ge_engine_math)
target_include_directories(ge_engine_math PUBLIC "include/")
target_link_libraries(ge_engine_math
    INTERFACE GE::RootConfig
    PUBLIC GE::Engine::Core
)
