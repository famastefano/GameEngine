#pragma once

#include "Core/BuildConfig.hpp"
#include "Core/Logging.hpp"

namespace Core::Private
{
CORE_API Core::Private::Logger& LogAssertRef();
}

#undef assert
#undef assertf
#undef assertNoEntry
#undef assertNoReentry
#undef assertNoRecursion
#undef expect
#undef expectf
#undef expectAlways
#undef expectAlwaysf
#undef verify
#undef verifyf
#undef LOG
#undef unimplemented

#define IMPL_GENERATE_UNIQUE_NAME() uniqueVariableName##__COUNTER__

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
#    define assertf(condition, ...)                                                                    \
        {                                                                                              \
            if(!(condition))                                                                           \
            {                                                                                          \
                ::Core::Private::LogAssertRef().log(Core::LogLevel::Fatal __VA_OPT__(, ) __VA_ARGS__); \
            }                                                                                          \
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

#    define assert(condition)   assertf((condition), L"Assertion failed: " #condition)
#    define assertNoEntry()     assertf(false, L"Assertion failed: entered forbidden code path.")
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
#    define IMPL_expectf(checkName, condition, ...)                                                       \
        {                                                                                                 \
            static bool checkName = false;                                                                \
            if(!checkName && !(condition))                                                                \
                ::Core::Private::LogAssertRef().log(Core::LogLevel::Warning, __VA_OPT__(, ) __VA_ARGS__); \
            checkName = true;                                                                             \
        }

#    define expectf(condition, ...) IMPL_expectf(IMPL_GENERATE_UNIQUE_NAME(), (condition), __VA_ARGS__)
#    define expect(condition)       expectf((condition), L"Expectation failed: " #condition)
#    define expectAlwaysf(condition, ...)                                                                 \
        {                                                                                                 \
            if(!(condition))                                                                              \
                ::Core::Private::LogAssertRef().log(Core::LogLevel::Warning, __VA_OPT__(, ) __VA_ARGS__); \
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
bool verifyfImpl(CondExpr&& cndExpr, wchar_t const* format, Args&&... args)
{
    static_assert(std::is_convertible_v<CondExpr, bool>, "verify/verifyf can't convert the expression to bool");
    bool condition = static_cast<bool>(cndExpr);
    if(!condition)
    {
        ::Core::Private::LogAssertRef().log(Core::LogLevel::Warning, format, std::forward<Args>(args)...);
    }
    return condition;
}
}
#    define verifyf(condition, format, ...) Private::verifyfImpl((condition), __VA_OPT__(, ) __VA_ARGS__)
#    define verify(condition)               verifyf((condition), "Verification failed: " #condition)
#else
#    define verify(condition)               condition
#    define verifyf(condition, format, ...) condition
#endif

#if ENABLE_LOGGING
#    define LOG(Category, Level, ...) Category.log(Level, __VA_ARGS__)
#else
#    define LOG(Category, Level, ...)
#endif

#define unimplemented() assertf(false, "Assertion failed: called unimplemented function.")
