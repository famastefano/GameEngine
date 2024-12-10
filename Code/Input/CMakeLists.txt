include_guard()

add_library(ge_engine_input
    "include/Input/API.h"
    
    "include/Input/Base/Events.h"
    "include/Input/Base/Translator.h"
)
if(WIN32)
    target_sources(ge_engine_input PRIVATE "src/Base/Win32Translator.cpp")
else()
    message(FATAL_ERROR "Only Windows is supported.")
endif()

add_library(GE::Engine::Input ALIAS ge_engine_input)
target_compile_definitions(ge_engine_input PRIVATE INPUT_API_EXPORTS)
target_include_directories(ge_engine_input PUBLIC "include/")
target_link_libraries(ge_engine_input
    INTERFACE GE::RootConfig
    PRIVATE GE::Engine::Core
)
