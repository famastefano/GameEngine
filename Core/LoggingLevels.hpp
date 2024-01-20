#pragma once

#include <cstdint>

namespace Core
{
enum class LogLevel : uint8_t
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};
}
