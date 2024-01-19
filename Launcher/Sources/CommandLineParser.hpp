#pragma once

#include "Core/AssertionMacros.hpp"
#include "Launcher/Sources/LogLauncher.hpp"

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <set>
#include <span>
#include <string_view>
#include <unordered_map>

template<typename CharType>
class CommandLineParser;

template<typename CharType>
class CommandLineParseResults
{
    friend class CommandLineParser<CharType>;

    public:
    using ViewType = typename std::basic_string_view<CharType>;

    private:
    std::set<std::size_t>                               hashedFlags;
    std::unordered_map<ViewType, std::vector<ViewType>> flagsWithParamList;

    public:
    bool isEmpty() const noexcept
    {
        return hashedFlags.empty() && flagsWithParamList.empty();
    }
    std::vector<ViewType> const* viewParamsOf(ViewType param) const noexcept
    {
        auto const it = flagsWithParamList.find(param);
        return it != flagsWithParamList.cend() ? &it->second : nullptr;
    }
    bool isSet(CharType flag) const noexcept
    {
        return hashedFlags.contains(std::hash<CharType>{}(flag));
    }
    bool areAllSet(ViewType flags) const noexcept
    {
        for(CharType flag: flags)
            if(!isSet(flag))
                return false;
        return true;
    }
    bool isAnySet(ViewType flags) const noexcept
    {
        for(CharType flag: flags)
            if(!isSet(flag))
                return true;
        return false;
    }
};

template<typename CharType>
class CommandLineParser
{
    static_assert(std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>);

    public:
    explicit CommandLineParser() = default;

    [[nodiscard]] CommandLineParseResults<CharType> parse(CharType** argv) noexcept
    {
        CommandLineParseResults<CharType> results;

        if(!argv)
            return results;

        using ViewType = typename std::basic_string_view<CharType>;

        auto isValidArgName = [](ViewType arg)
        {
            if constexpr(std::is_same_v<CharType, char>)
            {
                return std::ranges::all_of(arg, [](char c)
                                           { return std::isalnum(static_cast<unsigned char>(c)); });
            }
            else
            {
                return std::ranges::all_of(arg, [](wchar_t c)
                                           { return std::iswalnum(c); });
            }
        };

        while(*argv)
        {
            constexpr CharType prefix        = CharType('-');
            constexpr CharType separator     = CharType(',');
            constexpr CharType quote         = CharType('"');
            constexpr CharType paramListInit = CharType('=');

            ViewType arg{*argv};

            std::size_t const len = arg.length();
            if(len > 2 && arg[0] == prefix && arg[1] == prefix)
            {
                std::size_t const paramListInitPos = arg.find(paramListInit);
                ViewType          name             = arg.substr(2, paramListInitPos);
                if(isValidArgName(name))
                {
                    if(paramListInitPos != ViewType::npos)
                    {
                        // long flag like: `--help` or `--verbose`
                        results.hashedFlags.emplace(std::hash<ViewType>{}(name));
                    }
                    else
                    {
                        std::vector<ViewType> params;
                        std::size_t           curr = paramListInitPos;
                        do
                        {
                            std::size_t const next  = arg.find(separator, curr);
                            ViewType          param = arg.substr(curr, next);
                            if(param.starts_with(quote)) param.remove_prefix(1);
                            if(param.ends_with(quote)) param.remove_suffix(1);
                            params.emplace_back(param);
                            curr = next;
                        } while(curr != ViewType::npos);
                    }
                }
                else
                {
                    LOG(LogLauncher, Core::LogLevel::Warning, L"Invalid argument: {}", arg);
                }
            }
            else if(len > 1 && arg[0] == prefix)
            {
                // short flags like `-x` or `-xvf`
                ViewType name = arg.substr(1);
                if(isValidArgName(name))
                {
                    for(CharType c: arg)
                        results.hashedFlags.emplace(std::hash<CharType>{}(c));
                }
                else
                {
                    LOG(LogLauncher, Core::LogLevel::Warning, L"Invalid argument: {}", arg);
                }
            }
            ++argv;
        }

        return results;
    }
};