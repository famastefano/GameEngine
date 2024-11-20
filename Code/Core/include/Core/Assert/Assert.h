#pragma once

#include <Core/Logging/Logging.h>
#include <Core/Platform/Platform.h>

GE_DEFINE_INLINE_LOG_CATEGORY(LogAssert, Core::Verbosity::Warning)

// TODO: Add minidump on check(false)

#undef checkf
#undef check
#undef verifyf
#undef verify
#undef verifyOncef
#undef verifyOnce
#undef assert

#ifdef GE_BUILD_CONFIG_RELEASE // verify* macros can used inside if-statements, so we need to maintain the check
#  define checkf(Condition, Format, ...)
#  define check(Condition)
#  define verifyf(Condition, Format, ...) (Condition)
#  define verify(Condition)               (Condition)
#  define verifyOnce(Condition)           (Condition)
#  define assert(Condition)
#else
#  define checkf(Condition, Format, ...)                                                                              \
    if (!(Condition))                                                                                                 \
    {                                                                                                                 \
      GE_LOG(LogAssert, Core::Verbosity::Error, "Check failed: `" #Condition "` " Format __VA_OPT__(, ) __VA_ARGS__); \
      if (Core::IsDebuggerAttached())                                                                                 \
        __debugbreak();                                                                                               \
    }
#  define check(Condition) checkf(Condition, "")

#  define verifyf(Condition, Format, ...) [&](const bool v) {                                                            \
    if (!v)                                                                                                              \
      GE_LOG(LogAssert, Core::Verbosity::Warning, "Verify failed: `" #Condition "` " Format __VA_OPT__(, ) __VA_ARGS__); \
    return v;                                                                                                            \
  }((Condition))
#  define verify(Condition) verifyf(Condition, "")

#  define verifyOncef(Condition, Format, ...) [&](const bool v) {                                                        \
    static bool alreadyVerified = false;                                                                                 \
    if (!v && !alreadyVerified)                                                                                          \
      GE_LOG(LogAssert, Core::Verbosity::Warning, "Verify failed: `" #Condition "` " Format __VA_OPT__(, ) __VA_ARGS__); \
    alreadyVerified = true;                                                                                              \
    return v;                                                                                                            \
  }((Condition))
#  define verifyOnce(Condition) verifyOncef(Condition, "")

#  define assert(Condition) check(Condition)
#endif
