#pragma once

#include "Core/LoggingLevels.hpp"

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
    inline static std::unordered_map<std::wstring_view, Private::Logger*> loggers;

    public:
    static void registerLogger(std::wstring_view name, Private::Logger& logger) noexcept;
    static void unregisterLogger(std::wstring_view name) noexcept;
    static void changeLoggerLevel(std::wstring_view name, LogLevel newLevel) noexcept;
    static void flushAllLoggers() noexcept;
};
}