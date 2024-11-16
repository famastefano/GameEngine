#include <Core/Container/Vector.h>
#include <Core/Logging/Logging.h>
#include <Windows.h>
#include <cstdarg>
#include <cstdio>

namespace Core
{
#if !defined(GE_CORE_LOGGING_STATIC_FMT_BUFFER_SIZE) || (GE_CORE_LOGGING_STATIC_FMT_BUFFER_SIZE + 1) <= 1
#  undef GE_CORE_LOGGING_STATIC_FMT_BUFFER_SIZE
#  define GE_CORE_LOGGING_STATIC_FMT_BUFFER_SIZE 512
#endif

template <typename CharT>
void Log(LogCategoryBase& Category, Verbosity Verbosity, CharT const* fmt, va_list args)
{
  constexpr i32 MAX_SZ = GE_CORE_LOGGING_STATIC_FMT_BUFFER_SIZE;

  if((i32)Category.Verbosity_ > (i32)Verbosity)
    return;

  va_list postFmt;
  va_copy(postFmt, args);

  i32 requiredSz;
  if constexpr (std::same_as<CharT, char>)
    requiredSz = vsnprintf(nullptr, 0, fmt, args);
  else
    requiredSz = _vsnwprintf_s(nullptr, 0, 0, fmt, args);

  if (requiredSz < MAX_SZ)
  {
    CharT buf[MAX_SZ] = {};
    if constexpr (std::same_as<CharT, char>)
    {
      vsnprintf(buf, (u64)MAX_SZ, fmt, postFmt);
      OutputDebugStringA(buf);
    }
    else
    {
      _vsnwprintf_s(buf, (u64)MAX_SZ, (u64)requiredSz, fmt, postFmt);
      OutputDebugStringW(buf);
    }
    return;
  }

  Vector<CharT> buf(requiredSz + 1);
  if constexpr (std::same_as<CharT, char>)
  {
    vsnprintf(buf.Data(), (u64)MAX_SZ, fmt, postFmt);
    OutputDebugStringA(buf.Data());
  }
  else
  {
    _vsnwprintf_s(buf.Data(), (u64)buf.Size(), (u64)requiredSz, fmt, postFmt);
    OutputDebugStringW(buf.Data());
  }
}

void Log(LogCategoryBase& Category, Verbosity Verbosity, char const* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  Log<char>(Category, Verbosity, fmt, args);
}

void Log(LogCategoryBase& Category, Verbosity Verbosity, wchar_t const* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  Log<wchar_t>(Category, Verbosity, fmt, args);
}

char const* Private::ToString(Verbosity Verbosity, char const*)
{
  using enum Verbosity;
  switch (Verbosity)
  {
  case Off:
    return "Off";
  case Trace:
    return "Trace";
  case Debug:
    return "Debug";
  case Info:
    return "Info";
  case Warning:
    return "Warning";
  case Error:
    return "Error";
  }
  __assume(0);
}

wchar_t const* Private::ToString(Verbosity Verbosity, wchar_t const*)
{
  using enum Verbosity;
  switch (Verbosity)
  {
  case Off:
    return L"Off";
  case Trace:
    return L"Trace";
  case Debug:
    return L"Debug";
  case Info:
    return L"Info";
  case Warning:
    return L"Warning";
  case Error:
    return L"Error";
  }
  __assume(0);
}
} // namespace Core
