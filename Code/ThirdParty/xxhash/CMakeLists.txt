include_guard()

add_library(ge_3rdparty_xxhash STATIC
    "include/xxhash/xxhash.h"
    "src/xxhash.cpp"
)
add_library(GE::ThirdParty::xxhash ALIAS ge_3rdparty_xxhash)
target_link_libraries(ge_3rdparty_xxhash INTERFACE GE::RootConfig)
target_include_directories(ge_3rdparty_xxhash PUBLIC "include/")