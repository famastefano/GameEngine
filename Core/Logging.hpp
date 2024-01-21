#pragma once

#include "Core/BuildConfig.hpp"
#include "Core/LoggingLevels.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <format>
#include <memory>
#include <span>
#include <string>

template<>
struct std::formatter<char const*, wchar_t>
{
    bool quoted = false;

    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.end();
    }

    template<class FmtContext>
    FmtContext::iterator format(char const* text, FmtContext& ctx) const
    {
        if(text)
        {
            std::mbstate_t state{};
            auto           converter = [&state](char c)
            {
                wchar_t wc;
                std::mbrtowc(&wc, &c, sizeof(char), &state);
                return wc;
            };
            return std::transform(text, text + std::strlen(text), ctx.out(), converter);
        }
        return ctx.out();
    }
};

template<>
struct std::formatter<std::string, wchar_t>
{
    bool quoted = false;

    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.end();
    }

    template<class FmtContext>
    FmtContext::iterator format(std::string const& text, FmtContext& ctx) const
    {
        if(!text.empty())
        {
            std::mbstate_t state{};
            auto           converter = [&state](char c)
            {
                wchar_t wc;
                std::mbrtowc(&wc, &c, sizeof(char), &state);
                return wc;
            };
            return std::transform(text.cbegin(), text.cend(), ctx.out(), converter);
        }
        return ctx.out();
    }
};

template<>
struct std::formatter<std::string_view, wchar_t>
{
    bool quoted = false;

    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.end();
    }

    template<class FmtContext>
    FmtContext::iterator format(std::string_view text, FmtContext& ctx) const
    {
        if(!text.empty())
        {
            std::mbstate_t state{};
            auto           converter = [&state](char c)
            {
                wchar_t wc;
                std::mbrtowc(&wc, &c, sizeof(char), &state);
                return wc;
            };
            return std::ranges::transform(text.data(), text.data() + text.size(), ctx.out(), converter).out;
        }
        return ctx.out();
    }
};

template<>
struct std::formatter<Core::LogLevel, wchar_t>
{
    std::wstring fmt;

    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        std::wstring localFmt;
        auto         pos = ctx.begin();
        while(pos != ctx.end())
        {
            wchar_t c = wchar_t(*pos);
            if(c == L'}')
                break;
            ++pos;
            localFmt += c;
        }
        fmt = localFmt.empty() ? L"{}" : L"{:" + localFmt + L'}';
        return pos;
    }

    template<class FmtContext>
    FmtContext::iterator format(Core::LogLevel level, FmtContext& ctx) const
    {
        constexpr std::array<std::wstring_view, 6> levelTable{
            L"Trace",
            L"Debug",
            L"Info",
            L"Warning",
            L"Error",
            L"Fatal"};
        return std::vformat_to(ctx.out(), fmt, std::make_wformat_args(levelTable[static_cast<uint8_t>(level)]));
    }
};

namespace Core
{
namespace Private
{
    class Logger
    {
        std::wstring_view name;
        FILE*             file;
        LogLevel          currLevel;
        LogLevel          maxLevel;

        public:
        Logger(wchar_t const* name, LogLevel defaultLevel, LogLevel maximumLevel) noexcept;
        ~Logger();

        void     flush() noexcept;
        void     logText(LogLevel level, wchar_t const* text, std::size_t length) const noexcept;
        void     changeLevel(LogLevel newLoggingLevel) noexcept;
        LogLevel currentLevel() const noexcept;
        LogLevel maximumLevel() const noexcept;
        bool     canLog(LogLevel level) const noexcept;

        template<typename... Args>
        void log(LogLevel level, wchar_t const* format, Args&&... args) const noexcept
        {
            if(canLog(level))
            {
                auto const text{std::vformat(format, std::make_wformat_args(std::forward<Args>(args)...))};
                logText(level, text.data(), text.size());
            }
        }
    };
}
}

#if ENABLE_LOGGING
#    define DECLARE_LOGGER_CATEGORY_EXTERN(CategoryName, DefaultLoggingLevel, MaximumLoggingLevel)                                       \
        extern struct Logger_##CategoryName : public Core::Private::Logger                                                               \
        {                                                                                                                                \
            Logger_##CategoryName() noexcept : Core::Private::Logger(L"" #CategoryName, (DefaultLoggingLevel), (MaximumLoggingLevel)) {} \
        } CategoryName;

#    define DECLARE_LOGGER_CATEGORY(CategoryName, DefaultLoggingLevel, MaximumLoggingLevel)                                              \
        struct Logger_##CategoryName : public Core::Private::Logger                                                                      \
        {                                                                                                                                \
            Logger_##CategoryName() noexcept : Core::Private::Logger(L"" #CategoryName, (DefaultLoggingLevel), (MaximumLoggingLevel)) {} \
        } CategoryName;

#    define DEFINE_LOGGER_CATEGORY(CategoryName) struct Logger_##CategoryName CategoryName;
#else
#    define DECLARE_LOGGER_CATEGORY_EXTERN(CategoryName, DefaultLoggingLevel, MaximumLoggingLevel)
#    define DECLARE_LOGGER_CATEGORY(CategoryName, DefaultLoggingLevel, MaximumLoggingLevel)
#    define DEFINE_LOGGER_CATEGORY(CategoryName)
#endif
