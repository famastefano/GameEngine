#include "Core/Logging.hpp"

#include "Core/AssertionMacros.hpp"
#include "Core/LoggerManager.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <debugapi.h>
#include <filesystem>

namespace Core::Private
{
Logger::Logger(wchar_t const* name, LogLevel defaultLevel, LogLevel maximumLevel) noexcept
    : name(name)
    , currLevel(defaultLevel)
    , maxLevel(maximumLevel)
{
    auto const loggerPath = LoggerManager::folder() + L"/" + name + L".log";
    file                  = _wfopen(loggerPath.data(), L"ac, ccs=UTF-8");
    assertf(file != nullptr, L"Couldn't create logger {}", name);

    LoggerManager::registerLogger(name, *this);
}

Logger::~Logger()
{
    LoggerManager::unregisterLogger(name);
    fflush(file);
    fclose(file);
}

void Logger::flush() noexcept
{
    fflush(file);
}

void Logger::logText(LogLevel level, wchar_t const* text, std::size_t length) const noexcept
{
    if(canLog(level))
    {
        if(IsDebuggerPresent())
        {
            OutputDebugStringW(text);
            if(level == LogLevel::Fatal)
                DebugBreak();
        }

        if(file)
        {
            fwprintf(file, L"%*s", static_cast<int>(length), text);
            fwprintf(file, L"\n");
        }
    }
}

void Logger::changeLevel(LogLevel newLoggingLevel) noexcept
{
    if(static_cast<uint8_t>(newLoggingLevel) >= static_cast<uint8_t>(maxLevel))
        currLevel = newLoggingLevel;
}

LogLevel Logger::currentLevel() const noexcept
{
    return currLevel;
}

LogLevel Logger::maximumLevel() const noexcept
{
    return maxLevel;
}

bool Logger::canLog(LogLevel level) const noexcept
{
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(currLevel);
}
}