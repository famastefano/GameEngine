include_guard()

add_library(ge_engine_math INTERFACE
    "include/Math/Vector.h"
)

add_library(GE::Engine::Math ALIAS ge_engine_math)
target_include_directories(ge_engine_math INTERFACE "include/")
target_link_libraries(ge_engine_math
    INTERFACE GE::RootConfig
    INTERFACE GE::Engine::Core
)
