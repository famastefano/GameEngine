cmake_minimum_required(VERSION 3.10)

# Run version helper script
include(cmake/version.cmake)

project(Tracy LANGUAGES CXX VERSION ${TRACY_VERSION_STRING})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")

if(${BUILD_SHARED_LIBS})
	set(DEFAULT_STATIC OFF)
else()
	set(DEFAULT_STATIC ON)
endif()

option(TRACY_STATIC "Whether to build Tracy as a static library" ${DEFAULT_STATIC})

find_package(Threads REQUIRED)

set(TRACY_PUBLIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/public)

if(TRACY_STATIC)
	set(TRACY_VISIBILITY "STATIC")
else()
	set(TRACY_VISIBILITY "SHARED")
endif()

add_library(ge_3rdparty_tracy ${TRACY_VISIBILITY} "${TRACY_PUBLIC_DIR}/TracyClient.cpp")
target_compile_definitions(ge_3rdparty_tracy PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(ge_3rdparty_tracy PRIVATE /w /EHa-)

target_compile_features(ge_3rdparty_tracy PUBLIC cxx_std_11)

target_include_directories(ge_3rdparty_tracy SYSTEM PUBLIC
    $<BUILD_INTERFACE:${TRACY_PUBLIC_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ge_3rdparty_tracy
    PUBLIC
        Threads::Threads
        ${CMAKE_DL_LIBS}
)

add_library(GE::ThirdParty::TracyClient ALIAS ge_3rdparty_tracy)

macro(set_option option help value)
    option(${option} ${help} ${value})
    if(${option})
        message(STATUS "${option}: ON")
        target_compile_definitions(ge_3rdparty_tracy PUBLIC ${option})
    else()
        message(STATUS "${option}: OFF")
    endif()
endmacro()

set_option(TRACY_ENABLE "Enable profiling" ON)
set_option(TRACY_ON_DEMAND "On-demand profiling" OFF)
set_option(TRACY_CALLSTACK "Enforce callstack collection for tracy regions" OFF)
set_option(TRACY_NO_CALLSTACK "Disable all callstack related functionality" OFF)
set_option(TRACY_NO_CALLSTACK_INLINES "Disables the inline functions in callstacks" OFF)
set_option(TRACY_ONLY_LOCALHOST "Only listen on the localhost interface" OFF)
set_option(TRACY_NO_BROADCAST "Disable client discovery by broadcast to local network" OFF)
set_option(TRACY_ONLY_IPV4 "Tracy will only accept connections on IPv4 addresses (disable IPv6)" OFF)
set_option(TRACY_NO_CODE_TRANSFER "Disable collection of source code" OFF)
set_option(TRACY_NO_CONTEXT_SWITCH "Disable capture of context switches" OFF)
set_option(TRACY_NO_EXIT "Client executable does not exit until all profile data is sent to server" OFF)
set_option(TRACY_NO_SAMPLING "Disable call stack sampling" OFF)
set_option(TRACY_NO_VERIFY "Disable zone validation for C API" OFF)
set_option(TRACY_NO_VSYNC_CAPTURE "Disable capture of hardware Vsync events" OFF)
set_option(TRACY_NO_FRAME_IMAGE  "Disable the frame image support and its thread" OFF)
set_option(TRACY_NO_SYSTEM_TRACING  "Disable systrace sampling" OFF)
set_option(TRACY_PATCHABLE_NOPSLEDS  "Enable nopsleds for efficient patching by system-level tools (e.g. rr)" OFF)
set_option(TRACY_DELAYED_INIT "Enable delayed initialization of the library (init on first call)" OFF)
set_option(TRACY_MANUAL_LIFETIME "Enable the manual lifetime management of the profile" OFF)
set_option(TRACY_FIBERS "Enable fibers support" OFF)
set_option(TRACY_NO_CRASH_HANDLER "Disable crash handling" OFF)
set_option(TRACY_TIMER_FALLBACK "Use lower resolution timers" OFF)
set_option(TRACY_LIBUNWIND_BACKTRACE "Use libunwind backtracing where supported" OFF)
set_option(TRACY_SYMBOL_OFFLINE_RESOLVE "Instead of full runtime symbol resolution, only resolve the image path and offset to enable offline symbol resolution" OFF)
set_option(TRACY_LIBBACKTRACE_ELF_DYNLOAD_SUPPORT "Enable libbacktrace to support dynamically loaded elfs in symbol resolution resolution after the first symbol resolve operation" OFF)
set_option(TRACY_DEBUGINFOD "Enable debuginfod support" OFF)

# advanced
set_option(TRACY_VERBOSE "[advanced] Verbose output from the profiler" OFF)
mark_as_advanced(TRACY_VERBOSE)
set_option(TRACY_DEMANGLE "[advanced] Don't use default demangling function - You'll need to provide your own" OFF)
mark_as_advanced(TRACY_DEMANGLE)

if(NOT TRACY_STATIC)
    target_compile_definitions(ge_3rdparty_tracy PRIVATE TRACY_EXPORTS)
    target_compile_definitions(ge_3rdparty_tracy PUBLIC TRACY_IMPORTS)
endif()

set_target_properties(ge_3rdparty_tracy PROPERTIES VERSION ${PROJECT_VERSION})

add_subdirectory("profiler")