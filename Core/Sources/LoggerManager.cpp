#include "Core/LoggerManager.hpp"

#include "Core/Logging.hpp"

#include <chrono>
#include <filesystem>

namespace Core
{
void LoggerManager::registerLogger(std::wstring_view name, Private::Logger& logger) noexcept
{
    loggers[name] = &logger;
}

void LoggerManager::unregisterLogger(std::wstring_view name) noexcept
{
    loggers.erase(name);
}

std::wstring LoggerManager::makeLoggerFolder() noexcept
{
    auto now  = std::chrono::system_clock::now();
    auto path = std::filesystem::current_path() / L"logs" / std::format("{:%F_%H-%M-%S}", now);
    if(!std::filesystem::exists(path) && std::filesystem::create_directories(path))
        return path;
    return {}; // if folder creation fails, we'll log inside the executable folder
}

std::wstring const& LoggerManager::folder() noexcept
{
    return loggerFolder;
}

void LoggerManager::flushAllLoggers() noexcept
{
    for(auto& [name, logger]: loggers)
        logger->flush();
}

void LoggerManager::changeLoggerLevel(std::wstring_view name, LogLevel newLevel) noexcept
{
    if(auto it = loggers.find(name); it != loggers.end())
        it->second->changeLevel(newLevel);
}
}