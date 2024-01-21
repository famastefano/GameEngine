#pragma once

#include "Core/BuildConfig.hpp"
#include "Core/LoggingLevels.hpp"

#include <atomic>
#include <string_view>
#include <unordered_map>

namespace Core
{
namespace Private
{
    class Logger;
}

class LoggerManager
{
    static std::wstring makeLoggerFolder() noexcept;

    inline static std::unordered_map<std::wstring_view, Private::Logger*> loggers;
    inline static std::wstring                                            loggerFolder = makeLoggerFolder();

#if ENABLE_TESTS
    inline static std::atomic_flag globalErrorFlag = ATOMIC_FLAG_INIT;
#endif

    public:
    static std::wstring const& folder() noexcept;

    static void registerLogger(std::wstring_view name, Private::Logger& logger) noexcept;
    static void unregisterLogger(std::wstring_view name) noexcept;
    static void changeLoggerLevel(std::wstring_view name, LogLevel newLevel) noexcept;
    static void flushAllLoggers() noexcept;

#if ENABLE_TESTS
    static void setGlobalErrorFlag() noexcept;
    static bool isGlobalErrorSet() noexcept;
#endif
};
}
