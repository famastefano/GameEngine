#pragma once

#include <Core/API.h>
#include <Core/Definitions.h>

namespace Core
{
enum class Verbosity
{
  Off,
  Trace,
  Debug,
  Info,
  Warning,
  Error,
};
struct LogCategoryBase
{
  Core::Verbosity Verbosity_;
};
CORE_API void Log(LogCategoryBase& Category, Verbosity Verbosity, char const* fmt, ...);
CORE_API void Log(LogCategoryBase& Category, Verbosity Verbosity, wchar_t const* fmt, ...);

namespace Private
{
CORE_API char const*    ToString(Verbosity Verbosity, char const*);
CORE_API wchar_t const* ToString(Verbosity Verbosity, wchar_t const*);
} // namespace Private
} // namespace Core

#undef GE_LOG
#undef GE_LOGW
#undef GE_DECLARE_LOG_CATEGORY
#undef GE_DEFINE_LOG_CATEGORY

#define GE_LOG(Category, Verbosity, Format, ...) \
  Core::Log(Category, Verbosity, "[%s]" Format " at " __FUNCTION__ "\n", Core::Private::ToString(Verbosity, Format) __VA_OPT__(, ) __VA_ARGS__)

#define GE_LOGW(Category, Verbosity, Format, ...) \
  Core::Log(Category, Verbosity, L"[%s]" Format L" at " __FUNCTIONW__ L"\n", Core::Private::ToString(Verbosity, Format) __VA_OPT__(, ) __VA_ARGS__)

#ifndef GE_BUILD_CONFIG_RELEASE
#  define GE_DECLARE_LOG_CATEGORY(Category)                             \
    extern struct LogCategory_##Category : public Core::LogCategoryBase \
    {                                                                   \
      static char const* Category_;                                     \
    } Category;

#  define GE_DEFINE_LOG_CATEGORY(Category, DefaultVerbosity)            \
    static struct LogCategory_##Category : public Core::LogCategoryBase \
    {                                                                   \
      static char const* Category_;                                     \
    } Category{DefaultVerbosity};                                       \
    char const* LogCategory_##Category::Category_ = #Category;
#else
#  define GE_LOG(Category_, Verbosity_, Format_, ...)
#  define GE_LOGW(Category_, Verbosity_, Format_, ...)
#  define GE_DECLARE_LOG_CATEGORY(Category_)
#  define GE_DEFINE_LOG_CATEGORY(Category, DefaultVerbosity)
#endif
