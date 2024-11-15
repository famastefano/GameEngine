#pragma once

#include <Core/Logging/Logging.h>
#include <Core/Platform/Platform.h>

GE_DEFINE_INLINE_LOG_CATEGORY(LogAssert, Core::Verbosity::Warning)

// TODO: Add minidump on check(false) + msg fmt

#undef assert

#define checkf(Condition, Format, ...)                                                                              \
  if (!(Condition))                                                                                                 \
  {                                                                                                                 \
    GE_LOG(LogAssert, Core::Verbosity::Error, "Check failed: `" #Condition "` " Format __VA_OPT__(, ) __VA_ARGS__); \
    if (Core::IsDebuggerAttached())                                                                                 \
      __debugbreak();                                                                                               \
  }
#define check(Condition) checkf(Condition, "")

#define verifyf(Condition, Format, ...) [](const bool v) {                                                             \
  if (!v)                                                                                                              \
    GE_LOG(LogAssert, Core::Verbosity::Warning, "Verify failed: `" #Condition "` " Format __VA_OPT__(, ) __VA_ARGS__); \
  return v;                                                                                                            \
}((Condition))
#define verify(Condition) verifyf(Condition, "")

#define verifyOncef(Condition, Format, ...) [](const bool v) {                                                         \
  static bool alreadyVerified = false;                                                                                 \
  if (!v && !alreadyVerified)                                                                                          \
    GE_LOG(LogAssert, Core::Verbosity::Warning, "Verify failed: `" #Condition "` " Format __VA_OPT__(, ) __VA_ARGS__); \
  alreadyVerified = true;                                                                                              \
  return v;                                                                                                            \
}((Condition))
#define verifyOnce(Condition) verifyOncef(Condition, "")

#define assert(Condition) check(Condition)
