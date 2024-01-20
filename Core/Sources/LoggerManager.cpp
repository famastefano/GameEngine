#include "Core/LoggerManager.hpp"

#include "Core/Logging.hpp"

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