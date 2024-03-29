#pragma once

#include "Core/BuildConfig.hpp"
#include "Core/Logging.hpp"

#include <chrono>

#if ENABLE_LOGGING
namespace Core::Private
{
Core::Private::Logger& LogAssertRef();
}
#endif

#define IMPL_GENERATE_UNIQUE_NAME_COUNTER2(name, line) name##line
#define IMPL_GENERATE_UNIQUE_NAME_COUNTER(name, line)  IMPL_GENERATE_UNIQUE_NAME_COUNTER2(name, line)
#define IMPL_GENERATE_UNIQUE_NAME()                    IMPL_GENERATE_UNIQUE_NAME_COUNTER(uniqueVariableName, __COUNTER__)

#if ENABLE_LOGGING
#    define IMPL_LOG_ASSERT(Category, Level, format, ...)                    \
        Category.log(Level, L"{:%Y-%m-%d %T} {:<8} LogAssert " format L"\n", \
                     std::chrono::system_clock::now(),                       \
                     Level __VA_OPT__(, ) __VA_ARGS__)
#else
#    define IMPL_LOG_ASSERT(Category, Level, format, ...)
#endif

#if ENABLE_ASSERT
namespace Private
{
struct RecursiveScopeCounter
{
    int& counter;
    RecursiveScopeCounter(int& counter) noexcept
        : counter(counter)
    {
        ++counter;
    }
    ~RecursiveScopeCounter()
    {
        --counter;
    }
};
}
// TODO: assertf should log and start and notify the crash handler
#    define assertf(condition, format, ...)                                                                     \
        {                                                                                                       \
            if(!(condition))                                                                                    \
            {                                                                                                   \
                IMPL_LOG_ASSERT(::Core::Private::LogAssertRef(), ::Core::LogLevel::Fatal, format, __VA_ARGS__); \
            }                                                                                                   \
        }

#    define IMPL_assertNoReentry(counterName)                                                   \
        {                                                                                       \
            static int counterName = 0;                                                         \
            assertf((counterName)++ > 0), L"Assertion failed: re-entered forbidden code path.") \
        }

#    define IMPL_assertNoRecursion(counterName)                                           \
        static int counterName = 0;                                                       \
        assertf((counterName) == 0, L"Assertion failed: entered code path recursively."); \
        Private::RecursiveScopeCounter recCounter_##counterName{counterName};

#    define assert(condition) assertf((condition), L"Assertion failed: " #condition)
#    define assertNoEntry()                                                \
        assertf(false, L"Assertion failed: entered forbidden code path."); \
        __assume(0)
#    define assertNoReentry()   IMPL_assertNoReentry(IMPL_GENERATE_UNIQUE_NAME())
#    define assertNoRecursion() IMPL_assertNoRecursion(IMPL_GENERATE_UNIQUE_NAME())
#else
#    define assert(condition)
#    define assertf(condition, ...)
#    define assertNoEntry()
#    define assertNoReentry()
#    define assertNoRecursion()
#endif

#if ENABLE_EXPECT
#    define IMPL_expectf(checkName, condition, format, ...)                                                       \
        {                                                                                                         \
            static bool checkName = false;                                                                        \
            if(!checkName && !(condition))                                                                        \
            {                                                                                                     \
                IMPL_LOG_ASSERT(::Core::Private::LogAssertRef(), ::Core::LogLevel::Warning, format, __VA_ARGS__); \
            }                                                                                                     \
            checkName = true;                                                                                     \
        }

#    define expectf(condition, ...) IMPL_expectf(IMPL_GENERATE_UNIQUE_NAME(), (condition), __VA_ARGS__)
#    define expect(condition)       expectf((condition), L"Expectation failed: " #condition)
#    define expectAlwaysf(condition, format, ...)                                                           \
        if(!(condition))                                                                                    \
        {                                                                                                   \
            IMPL_LOG_ASSERT(::Core::Private::LogAssertRef(), ::Core::LogLevel::Fatal, format, __VA_ARGS__); \
        }
#    define expectAlways(condition) expectAlwaysf((condition), L"Expectation failed: " #condition)
#else
#    define expect(condition)             condition
#    define expectf(condition, ...)       condition
#    define expectAlways(condition)       condition
#    define expectAlwaysf(condition, ...) condition
#endif

#if ENABLE_VERIFY
namespace Private
{
template<typename CondExpr, typename... Args>
bool verifyfImpl(::Core::LogLevel level, CondExpr&& cndExpr, wchar_t const* format, Args&&... args)
{
    static_assert(std::is_convertible_v<CondExpr, bool>, L"verify/verifyf can't convert the expression to bool");
    bool condition = static_cast<bool>(cndExpr);
    if(!condition)
    {
        ::Core::Private::LogAssertRef().log(level, format, std::forward<Args>(args)...);
    }
    return condition;
}
}
#    define verifyf(condition, format, ...) \
        Private::verifyfImpl(::Core::LogLevel::Warning, (condition), L"{:%Y-%m-%d %T} {:<8} LogAssert " format L"\n", std::chrono::system_clock::now(), ::Core::LogLevel::Warning __VA_OPT__(, ) __VA_ARGS__)
#    define verify(condition) verifyf((condition), L"Verification failed: " #condition)
#else
#    define verify(condition)               condition
#    define verifyf(condition, format, ...) condition
#endif

#define unimplemented() assertf(false, L"Assertion failed: called unimplemented function.")
