include_guard()

add_library(ge_engine_shared ${GE_BUILD_ADD_LIBRARY_TYPE}
    "include/Shared/API.h"

    "include/Shared/Events/EventBase.h"
    "src/EventBase.cpp"

    "include/Shared/Interfaces/IGameEngine.h"
    "include/Shared/Interfaces/ISubSystem.h"
    "include/Shared/Interfaces/IEnvironment.h"
    "src/IEnvironment.cpp"
)
add_library(GE::Engine::Shared ALIAS ge_engine_shared)
target_include_directories(ge_engine_shared PUBLIC "include/")
target_compile_definitions(ge_engine_shared PRIVATE SHARED_API_EXPORTS)
target_link_libraries(ge_engine_shared
    INTERFACE ge_root_config_target
    PUBLIC GE::Engine::Core
)