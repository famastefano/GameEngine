include_guard()

add_library(ge_engine_core ${GE_BUILD_ADD_LIBRARY_TYPE}
    "src/StaticInit.cpp"

    "include/Core/API.h"
    "include/Core/Definitions.h"
    "include/Core/Helpers.h"

    "include/Core/Algorithm/Algorithm.h"
    
    "include/Core/Allocator/Allocator.h"
    "include/Core/Allocator/ArenaAllocator.h"
    "include/Core/Allocator/GlobalAllocator.h"
    "src/Allocator/GlobalAllocator.cpp"
    
    "include/Core/Assert/Assert.h"
    
    "include/Core/Concepts/Concepts.h"

    "include/Core/Container/FlatMap.h"
    "include/Core/Container/Span.h"
    "include/Core/Container/String.h"
    "include/Core/Container/Vector.h"

    "include/Core/Functional/Delegate.h"

    "include/Core/Hash/Hash.h"
    "src/Hash/Hash.cpp"
    "src/Hash/xxhash.h"
    "src/Hash/xxhash.c"

    "include/Core/Iterator/ContiguousIterator.h"

    "include/Core/Logging/Logging.h"
    "src/Logging/Logging.cpp"
    "include/Core/Logging/Private/CoreLogging.h"
    "src/Logging/CoreLogging.cpp"

    "include/Core/Platform/Platform.h"
    "src/Platform/Platform.cpp"

    "include/Core/Reflection/Reflection.h"
)
add_library(GE::Engine::Core ALIAS ge_engine_core)
target_compile_definitions(ge_engine_core PRIVATE CORE_API_EXPORTS)
target_include_directories(ge_engine_core PUBLIC "include/")
target_link_libraries(ge_engine_core INTERFACE GE::RootConfig)

if(${GE_BUILD_ENABLE_TESTS})
    add_subdirectory("tests")
endif()