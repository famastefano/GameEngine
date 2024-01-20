#include "Launcher/Sources/CommandLineParser.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <memory>
#include <string>

template<typename CharType>
class ArgvHelper
{
    using StringType = typename std::basic_string<CharType>;
    std::vector<CharType*> args;

    public:
    CharType** argv()
    {
        args.emplace_back(nullptr);
        return !args.empty() ? args.data() : nullptr;
    }

    template<std::size_t N>
    ArgvHelper& append(char const (&param)[N])
    {
        using Traits = typename std::char_traits<CharType>;
        auto buf     = std::make_unique_for_overwrite<CharType[]>(N);
        for(std::size_t i = 0; i < N; ++i)
            buf[i] = Traits::to_char_type(param[i]);
        args.emplace_back(buf.release());
        return *this;
    }

    ~ArgvHelper()
    {
        for(CharType* str: args)
            delete[] str;
    }
};

template<typename CharType, std::size_t N>
std::basic_string<CharType> makeString(char const (&param)[N])
{
    using Traits = typename std::char_traits<CharType>;
    std::basic_string<CharType> str(N - 1, CharType('\0'));
    for(std::size_t i = 0; i < N - 1; ++i)
        str[i] = Traits::to_char_type(param[i]);
    return str;
}

TEMPLATE_TEST_CASE("Using the Command Line Parser", "[launcher]", char, wchar_t)
{
    using Parser     = CommandLineParser<TestType>;
    using Results    = CommandLineParseResults<TestType>;
    using Helper     = ArgvHelper<TestType>;
    using StringType = std::basic_string<TestType>;

    Parser parser;
    Helper helper;

    SECTION("The results are empty")
    {
        SECTION("if no arguments are provided")
        {
            Results const params = parser.parse(helper.argv());
            REQUIRE(params.isEmpty());
        }

        SECTION("if the only argument is the executable path")
        {
            Results const params = parser.parse(helper.append(__FILE__).argv());
            REQUIRE(params.isEmpty());
        }

        SECTION("if no argument starts with the proper prefix")
        {
            Results const params = parser.parse(helper
                                                    .append(__FILE__)
                                                    .append(__FUNCTION__)
                                                    .append("_-_something")
                                                    .append("3-17-59")
                                                    .append("//-////")
                                                    .append("\\\\-\\=")
                                                    .append("====    ")
                                                    .argv());
            REQUIRE(params.isEmpty());
        }

        SECTION("if the arguments are just the prefixes")
        {
            Results const params = parser.parse(helper
                                                    .append("-")
                                                    .append("--")
                                                    .append("- - -")
                                                    .append("-- -- --")
                                                    .argv());
            REQUIRE(params.isEmpty());
        }
    }

    SECTION("The results contains the short flags")
    {
        SECTION("if one flag is exist")
        {
            Results const params = parser.parse(helper.append("-x").argv());
            auto          xstr   = makeString<TestType>("x");
            REQUIRE(params.isSet(TestType('x')));
            REQUIRE(params.isAnySet(xstr));
            REQUIRE(params.areAllSet(xstr));
        }

        SECTION("if multiple flags as the same parameter are passed")
        {
            Results const params = parser.parse(helper.append("-xvf").argv());
            auto          xvf    = makeString<TestType>("xvf");
            auto          vf     = makeString<TestType>("vf");
            auto          fv     = makeString<TestType>("fv");
            REQUIRE(params.isSet(TestType('x')));
            REQUIRE(params.isSet(TestType('v')));
            REQUIRE(params.isSet(TestType('f')));
            REQUIRE(params.isAnySet(xvf));
            REQUIRE(params.areAllSet(xvf));
            REQUIRE(params.areAllSet(vf));
            REQUIRE(params.areAllSet(fv));
        }

        SECTION("if multiple flags as different parameters are passed")
        {
            Results const params = parser.parse(helper
                                                    .append("-x")
                                                    .append("-v")
                                                    .append("-f")
                                                    .argv());
            auto          xvf    = makeString<TestType>("xvf");
            auto          vf     = makeString<TestType>("vf");
            auto          fv     = makeString<TestType>("fv");
            REQUIRE(params.isSet(TestType('x')));
            REQUIRE(params.isSet(TestType('v')));
            REQUIRE(params.isSet(TestType('f')));
            REQUIRE(params.isAnySet(xvf));
            REQUIRE(params.areAllSet(xvf));
            REQUIRE(params.areAllSet(vf));
            REQUIRE(params.areAllSet(fv));
        }

        SECTION("if multiple parameters are passed, and some are contiguous")
        {
            Results const params = parser.parse(helper
                                                    .append("-x")
                                                    .append("-xvf")
                                                    .append("-fqt")
                                                    .append("-pxq")
                                                    .append("-nsb")
                                                    .argv());
            auto          str    = makeString<TestType>("xvfqtpq");
            REQUIRE(params.areAllSet(str));
        }
    }

    SECTION("The results contains the verbose flags with no parameters")
    {
        SECTION("if a single flag is present")
        {
            Results const params = parser.parse(helper.append("--help").argv());
            auto          str    = makeString<TestType>("help");
            REQUIRE_FALSE(params.isAnySet(str));
            REQUIRE(params.isSet(str));
        }

        SECTION("if multiple unique flags are present")
        {
            Results const params  = parser.parse(helper
                                                     .append("--help")
                                                     .append("--verbose")
                                                     .append("--flush")
                                                     .argv());
            auto          help    = makeString<TestType>("help");
            auto          verbose = makeString<TestType>("verbose");
            auto          flush   = makeString<TestType>("flush");
            REQUIRE(params.isSet(help));
            REQUIRE(params.isSet(verbose));
            REQUIRE(params.isSet(flush));
        }
    }

    SECTION("The results contains the verbose flags with a parameter list")
    {
        SECTION("if it has at least 1 parameter")
        {
            Results const params        = parser.parse(helper
                                                           .append("--LogAssert=Trace")
                                                           .argv());
            auto          _logAssertStr = makeString<TestType>("LogAssert");
            auto          _traceStr     = makeString<TestType>("Trace");
            REQUIRE(params.isSet(_logAssertStr));
            auto const* list = params.viewParamsOf(_logAssertStr);
            REQUIRE(list != nullptr);
            REQUIRE(list->size() == 1);
            REQUIRE(list->front() == _traceStr);
        }

        SECTION("if it has a single parameter with quotes, they aren't included")
        {
            Results const params      = parser.parse(helper
                                                         .append("--I=\"/usr/bin/long path name/test/strange naming convention folder\"")
                                                         .argv());
            auto          _includeStr = makeString<TestType>("I");
            auto          _path       = makeString<TestType>("/usr/bin/long path name/test/strange naming convention folder");
            REQUIRE(params.isSet(_includeStr));
            auto const* list = params.viewParamsOf(_includeStr);
            REQUIRE(list != nullptr);
            REQUIRE(list->size() == 1);
            REQUIRE(list->front() == _path);
        }

        SECTION("if a flag has multiple parameters")
        {
            Results const params    = parser.parse(helper
                                                       .append(R"(--folder="C:\Program Files\",/usr/bin,"ftp://10.0.0.0/secret/")")
                                                       .argv());
            auto          _folder   = makeString<TestType>("folder");
            StringType    folders[] = {
                makeString<TestType>(R"(C:\Program Files\)"),
                makeString<TestType>(R"(/usr/bin)"),
                makeString<TestType>(R"(ftp://10.0.0.0/secret/)"),
            };
            REQUIRE(params.isSet(_folder));
            auto const* list = params.viewParamsOf(_folder);
            REQUIRE(list != nullptr);
            REQUIRE(list->size() == 3);
            REQUIRE((*list)[0] == folders[0]);
            REQUIRE((*list)[1] == folders[1]);
            REQUIRE((*list)[2] == folders[2]);
        }

        SECTION("if multiple flags are repeated, their parameters are combined")
        {
            Results const params    = parser.parse(helper
                                                       .append("--I=/usr/include")
                                                       .append("--I=/usr/bin")
                                                       .append("--I=/usr/share")
                                                       .argv());
            auto          _include  = makeString<TestType>("I");
            StringType    folders[] = {
                makeString<TestType>("/usr/include"),
                makeString<TestType>("/usr/bin"),
                makeString<TestType>("/usr/share"),
            };
            REQUIRE(params.isSet(_include));
            auto const* list = params.viewParamsOf(_include);
            REQUIRE(list != nullptr);
            REQUIRE(list->size() == 3);
            REQUIRE((*list)[0] == folders[0]);
            REQUIRE((*list)[1] == folders[1]);
            REQUIRE((*list)[2] == folders[2]);
        }

        SECTION("if multiple unique flags exists, each one has its own list")
        {
            Results const params = parser.parse(helper
                                                    .append("--I=/usr/include")
                                                    .append("--I=/usr/bin")
                                                    .append("--I=/usr/share")
                                                    .append("--help")
                                                    .append("--verbose")
                                                    .append("--flush")
                                                    .append("--LogAssert=Trace")
                                                    .append("--I=\"/usr/bin/long path name/test/strange naming convention folder")
                                                    .append(R"(--folder="C:\Program Files\",/usr/bin,"ftp://10.0.0.0/secret/")")
                                                    .argv());
            // --I
            {
                auto       _include  = makeString<TestType>("I");
                StringType folders[] = {
                    makeString<TestType>("/usr/include"),
                    makeString<TestType>("/usr/bin"),
                    makeString<TestType>("/usr/share"),
                    makeString<TestType>("/usr/bin/long path name/test/strange naming convention folder"),
                };
                REQUIRE(params.isSet(_include));
                auto const* list = params.viewParamsOf(_include);
                REQUIRE(list != nullptr);
                REQUIRE(list->size() == 4);
                REQUIRE((*list)[0] == folders[0]);
                REQUIRE((*list)[1] == folders[1]);
                REQUIRE((*list)[2] == folders[2]);
                REQUIRE((*list)[3] == folders[3]);
            }

            // --help
            {
                auto _help = makeString<TestType>("help");
                REQUIRE(params.isSet(_help));
                REQUIRE(params.viewParamsOf(_help) == nullptr);
            }

            // --verbose
            {
                auto _verbose = makeString<TestType>("verbose");
                REQUIRE(params.isSet(_verbose));
                REQUIRE(params.viewParamsOf(_verbose) == nullptr);
            }

            // --flush
            {
                auto _flush = makeString<TestType>("flush");
                REQUIRE(params.isSet(_flush));
                REQUIRE(params.viewParamsOf(_flush) == nullptr);
            }

            // --LogAssert
            {
                auto _logAssertStr = makeString<TestType>("LogAssert");
                auto _traceStr     = makeString<TestType>("Trace");
                REQUIRE(params.isSet(_logAssertStr));
                auto const* list = params.viewParamsOf(_logAssertStr);
                REQUIRE(list != nullptr);
                REQUIRE(list->size() == 1);
                REQUIRE(list->front() == _traceStr);
            }
        }
    }

    SECTION("When edge cases occur, like")
    {
        SECTION("a param list composed of separators only should be parsed as an empty list")
        {
            Results const params = parser.parse(helper.append("--test=,,,,,,,").argv());
            auto          _test  = makeString<TestType>("test");
            REQUIRE(params.isSet(_test));
            REQUIRE(params.viewParamsOf(_test) == nullptr);
        }

        SECTION("an empty param list should add the flag but with an empty list")
        {
            Results const params = parser.parse(helper.append("--test=").argv());
            auto          _test  = makeString<TestType>("test");
            REQUIRE(params.isSet(_test));
            REQUIRE(params.viewParamsOf(_test) == nullptr);
        }

        SECTION("a long flag containing the prefix shouldn't be added")
        {
            Results const params = parser.parse(helper.append("--abcde--bc").argv());
            auto          _arg   = makeString<TestType>("abcde--bc");
            REQUIRE_FALSE(params.isSet(_arg));
            REQUIRE(params.viewParamsOf(_arg) == nullptr);
        }

        SECTION("a long flag being just the prefix shouldn't be added")
        {
            Results const params = parser.parse(helper.append("---").argv());
            auto          _arg   = makeString<TestType>("-");
            REQUIRE_FALSE(params.isSet(_arg));
            REQUIRE(params.viewParamsOf(_arg) == nullptr);
        }

        SECTION("a long flag with the parameter list containing the prefix should be valid")
        {
            Results const params = parser.parse(helper.append("--what=-is-it").argv());
            auto          _what  = makeString<TestType>("what");
            auto          _is_it = makeString<TestType>("-is-it");
            REQUIRE(params.isSet(_what));
            auto const* list = params.viewParamsOf(_what);
            REQUIRE(list != nullptr);
            REQUIRE(list->size() == 1);
            REQUIRE(list->front() == _is_it);
        }

        SECTION("a param list containing only empty quoted strings shall be parsed as an empty list")
        {
            Results const params = parser.parse(helper.append(R"(--arg="","","",)").argv());
            auto          _arg   = makeString<TestType>("arg");
            REQUIRE(params.isSet(_arg));
            REQUIRE(params.viewParamsOf(_arg) == nullptr);
        }

        SECTION("a param list where the separator is quoted shall add the separators to the list")
        {
            Results const params = parser.parse(helper.append(R"(--arg=",",",",",",)").argv());
            auto          _arg   = makeString<TestType>("arg");
            auto          _sep   = makeString<TestType>(",");
            REQUIRE(params.isSet(_arg));
            auto const* list = params.viewParamsOf(_arg);
            REQUIRE(list != nullptr);
            REQUIRE(list->size() == 3);
            REQUIRE((*list)[0] == _sep);
            REQUIRE((*list)[1] == _sep);
            REQUIRE((*list)[2] == _sep);
        }
    }
}
