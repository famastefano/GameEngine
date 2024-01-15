#include "Core/AssertionMacros.hpp"
#include "LogCore.hpp"

Core::Private::Logger& Core::Private::LogAssertRef()
{
    return LogAssert;
}
