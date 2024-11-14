include_guard()

add_library(ge_3rdparty_rapidjson INTERFACE)
file(GLOB_RECURSE GE_3RDPARTY_RAPIDJSON_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "include/*.h")
target_sources(ge_3rdparty_rapidjson PUBLIC ${GE_3RDPARTY_RAPIDJSON_SOURCES})
add_library(GE::ThirdParty::RapidJson ALIAS ge_3rdparty_rapidjson)
target_link_libraries(ge_3rdparty_rapidjson INTERFACE GE::RootConfig)
target_include_directories(ge_3rdparty_rapidjson INTERFACE "include/")