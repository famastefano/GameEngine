#include "Launcher/Sources/CommandLineParser.hpp"
#include "TestCommon/CommonMacros.hpp"

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
            GE_TEST_TRUE(params.isEmpty());
        }

        SECTION("if the only argument is the executable path")
        {
            Results const params = parser.parse(helper.append(__FILE__).argv());
            GE_TEST_TRUE(params.isEmpty());
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
            GE_TEST_TRUE(params.isEmpty());
        }

        SECTION("if the arguments are just the prefixes")
        {
            Results const params = parser.parse(helper
                                                    .append("-")
                                                    .append("--")
                                                    .append("- - -")
                                                    .append("-- -- --")
                                                    .argv());
            GE_TEST_TRUE(params.isEmpty());
        }
    }

    SECTION("The results contains the short flags")
    {
        SECTION("if one flag is exist")
        {
            Results const params = parser.parse(helper.append("-x").argv());
            auto          xstr   = makeString<TestType>("x");
            GE_TEST_TRUE(params.isSet(TestType('x')));
            GE_TEST_TRUE(params.isAnySet(xstr));
            GE_TEST_TRUE(params.areAllSet(xstr));
        }

        SECTION("if multiple flags as the same parameter are passed")
        {
            Results const params = parser.parse(helper.append("-xvf").argv());
            auto          xvf    = makeString<TestType>("xvf");
            auto          vf     = makeString<TestType>("vf");
            auto          fv     = makeString<TestType>("fv");
            GE_TEST_TRUE(params.isSet(TestType('x')));
            GE_TEST_TRUE(params.isSet(TestType('v')));
            GE_TEST_TRUE(params.isSet(TestType('f')));
            GE_TEST_TRUE(params.isAnySet(xvf));
            GE_TEST_TRUE(params.areAllSet(xvf));
            GE_TEST_TRUE(params.areAllSet(vf));
            GE_TEST_TRUE(params.areAllSet(fv));
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
            GE_TEST_TRUE(params.isSet(TestType('x')));
            GE_TEST_TRUE(params.isSet(TestType('v')));
            GE_TEST_TRUE(params.isSet(TestType('f')));
            GE_TEST_TRUE(params.isAnySet(xvf));
            GE_TEST_TRUE(params.areAllSet(xvf));
            GE_TEST_TRUE(params.areAllSet(vf));
            GE_TEST_TRUE(params.areAllSet(fv));
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
            GE_TEST_TRUE(params.areAllSet(str));
        }
    }

    SECTION("The results contains the verbose flags with no parameters")
    {
        SECTION("if a single flag is present")
        {
            Results const params = parser.parse(helper.append("--help").argv());
            auto          str    = makeString<TestType>("help");
            GE_TEST_TRUE(!params.isAnySet(str));
            GE_TEST_TRUE(params.isSet(str));
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
            GE_TEST_TRUE(params.isSet(help));
            GE_TEST_TRUE(params.isSet(verbose));
            GE_TEST_TRUE(params.isSet(flush));
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
            GE_TEST_TRUE(params.isSet(_logAssertStr));
            auto const* list = params.viewParamsOf(_logAssertStr);
            GE_TEST_TRUE(list != nullptr);
            GE_TEST_TRUE(list->size() == 1);
            GE_TEST_TRUE(list->front() == _traceStr);
        }

        SECTION("if it has a single parameter with quotes, they aren't included")
        {
            Results const params      = parser.parse(helper
                                                         .append("--I=\"/usr/bin/long path name/test/strange naming convention folder\"")
                                                         .argv());
            auto          _includeStr = makeString<TestType>("I");
            auto          _path       = makeString<TestType>("/usr/bin/long path name/test/strange naming convention folder");
            GE_TEST_TRUE(params.isSet(_includeStr));
            auto const* list = params.viewParamsOf(_includeStr);
            GE_TEST_TRUE(list != nullptr);
            GE_TEST_TRUE(list->size() == 1);
            GE_TEST_TRUE(list->front() == _path);
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
            GE_TEST_TRUE(params.isSet(_folder));
            auto const* list = params.viewParamsOf(_folder);
            GE_TEST_TRUE(list != nullptr);
            GE_TEST_TRUE(list->size() == 3);
            GE_TEST_TRUE((*list)[0] == folders[0]);
            GE_TEST_TRUE((*list)[1] == folders[1]);
            GE_TEST_TRUE((*list)[2] == folders[2]);
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
            GE_TEST_TRUE(params.isSet(_include));
            auto const* list = params.viewParamsOf(_include);
            GE_TEST_TRUE(list != nullptr);
            GE_TEST_TRUE(list->size() == 3);
            GE_TEST_TRUE((*list)[0] == folders[0]);
            GE_TEST_TRUE((*list)[1] == folders[1]);
            GE_TEST_TRUE((*list)[2] == folders[2]);
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
                GE_TEST_TRUE(params.isSet(_include));
                auto const* list = params.viewParamsOf(_include);
                GE_TEST_TRUE(list != nullptr);
                GE_TEST_TRUE(list->size() == 4);
                GE_TEST_TRUE((*list)[0] == folders[0]);
                GE_TEST_TRUE((*list)[1] == folders[1]);
                GE_TEST_TRUE((*list)[2] == folders[2]);
                GE_TEST_TRUE((*list)[3] == folders[3]);
            }

            // --help
            {
                auto _help = makeString<TestType>("help");
                GE_TEST_TRUE(params.isSet(_help));
                GE_TEST_TRUE(params.viewParamsOf(_help) == nullptr);
            }

            // --verbose
            {
                auto _verbose = makeString<TestType>("verbose");
                GE_TEST_TRUE(params.isSet(_verbose));
                GE_TEST_TRUE(params.viewParamsOf(_verbose) == nullptr);
            }

            // --flush
            {
                auto _flush = makeString<TestType>("flush");
                GE_TEST_TRUE(params.isSet(_flush));
                GE_TEST_TRUE(params.viewParamsOf(_flush) == nullptr);
            }

            // --LogAssert
            {
                auto _logAssertStr = makeString<TestType>("LogAssert");
                auto _traceStr     = makeString<TestType>("Trace");
                GE_TEST_TRUE(params.isSet(_logAssertStr));
                auto const* list = params.viewParamsOf(_logAssertStr);
                GE_TEST_TRUE(list != nullptr);
                GE_TEST_TRUE(list->size() == 1);
                GE_TEST_TRUE(list->front() == _traceStr);
            }
        }
    }

    SECTION("When edge cases occur, like")
    {
        SECTION("a param list composed of separators only should be parsed as an empty list")
        {
            Results const params = parser.parse(helper.append("--test=,,,,,,,").argv());
            auto          _test  = makeString<TestType>("test");
            GE_TEST_TRUE(params.isSet(_test));
            GE_TEST_TRUE(params.viewParamsOf(_test) == nullptr);
        }

        SECTION("an empty param list should add the flag but with an empty list")
        {
            Results const params = parser.parse(helper.append("--test=").argv());
            auto          _test  = makeString<TestType>("test");
            GE_TEST_TRUE(params.isSet(_test));
            GE_TEST_TRUE(params.viewParamsOf(_test) == nullptr);
        }

        SECTION("a long flag containing the prefix shouldn't be added")
        {
            Results const params = parser.parse(helper.append("--abcde--bc").argv());
            auto          _arg   = makeString<TestType>("abcde--bc");
            GE_TEST_TRUE(!params.isSet(_arg));
            GE_TEST_TRUE(params.viewParamsOf(_arg) == nullptr);
        }

        SECTION("a long flag being just the prefix shouldn't be added")
        {
            Results const params = parser.parse(helper.append("---").argv());
            auto          _arg   = makeString<TestType>("-");
            GE_TEST_TRUE(!params.isSet(_arg));
            GE_TEST_TRUE(params.viewParamsOf(_arg) == nullptr);
        }

        SECTION("a long flag with the parameter list containing the prefix should be valid")
        {
            Results const params = parser.parse(helper.append("--what=-is-it").argv());
            auto          _what  = makeString<TestType>("what");
            auto          _is_it = makeString<TestType>("-is-it");
            GE_TEST_TRUE(params.isSet(_what));
            auto const* list = params.viewParamsOf(_what);
            GE_TEST_TRUE(list != nullptr);
            GE_TEST_TRUE(list->size() == 1);
            GE_TEST_TRUE(list->front() == _is_it);
        }

        SECTION("a param list containing only empty quoted strings shall be parsed as an empty list")
        {
            Results const params = parser.parse(helper.append(R"(--arg="","","",)").argv());
            auto          _arg   = makeString<TestType>("arg");
            GE_TEST_TRUE(params.isSet(_arg));
            GE_TEST_TRUE(params.viewParamsOf(_arg) == nullptr);
        }

        SECTION("a param list where the separator is quoted shall add the separators to the list")
        {
            Results const params = parser.parse(helper.append(R"(--arg=",",",",",",)").argv());
            auto          _arg   = makeString<TestType>("arg");
            auto          _sep   = makeString<TestType>(",");
            GE_TEST_TRUE(params.isSet(_arg));
            auto const* list = params.viewParamsOf(_arg);
            GE_TEST_TRUE(list != nullptr);
            GE_TEST_TRUE(list->size() == 3);
            GE_TEST_TRUE((*list)[0] == _sep);
            GE_TEST_TRUE((*list)[1] == _sep);
            GE_TEST_TRUE((*list)[2] == _sep);
        }
    }
}
