#pragma once

#include <Core/Logging/Logging.h>
#include <Core/Platform/Platform.h>

GE_DEFINE_INLINE_LOG_CATEGORY(LogAssert, Core::Verbosity::Warning)

// TODO: Add minidump on check(false) + msg fmt

#undef assert

#define check(cnd, ...)                                                                               \
  if (!(cnd))                                                                                         \
  {                                                                                                   \
    GE_LOG(LogAssert, Core::Verbosity::Error, "Check failed: `" #cnd "`" __VA_OPT__(, ) __VA_ARGS__); \
    if (Core::IsDebuggerAttached())                                                                   \
      __debugbreak();                                                                                 \
  }

#define verify(cnd, ...) [](const bool v) {                                                              \
  if (!v)                                                                                                \
    GE_LOG(LogAssert, Core::Verbosity::Warning, "Verify failed: `" #cnd "`" __VA_OPT__(, ) __VA_ARGS__); \
  return v;                                                                                              \
}((cnd))

#define assert(...) check(__VA_ARGS__)
