#pragma once

#include "Core/AssertionMacros.hpp"
#include "Launcher/Sources/LogLauncher.hpp"

#include <string_view>

template<typename CharType>
class CommandLineParser;

template<typename CharType>
class CommandLineParseResults
{
    friend class CommandLineParser<CharType>;

    public:
    using ViewType = typename std::basic_string_view<CharType>;

    bool isEmpty() const noexcept
    {
        unimplemented();
        return false;
    }
    bool viewParamsOf(ViewType param) const noexcept
    {
        unimplemented();
    }
    bool isSet(CharType flag) const noexcept
    {
        unimplemented();
    }
    bool areAllSet(ViewType flags) const noexcept
    {
        unimplemented();
    }
    bool isAnySet(ViewType flags) const noexcept
    {
        unimplemented();
    }
};

template<typename CharType>
class CommandLineParser
{
    public:
    explicit CommandLineParser() = default;

    [[nodiscard]] CommandLineParseResults<CharType> parse(CharType** argv) noexcept
    {
        (void)(argv);
        unimplemented();
        return {};
    }
};