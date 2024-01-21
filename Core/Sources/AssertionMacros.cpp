#include "Core/AssertionMacros.hpp"
#include "LogCore.hpp"

#if ENABLE_LOGGING
Core::Private::Logger& Core::Private::LogAssertRef()
{
    return LogAssert;
}
#endif