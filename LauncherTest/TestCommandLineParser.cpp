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

TEMPLATE_TEST_CASE("Using the Command Line Parser", "[launcher]", char, wchar_t)
{
    using Parser  = CommandLineParser<TestType>;
    using Results = CommandLineParseResults<TestType>;
    using Helper  = ArgvHelper<TestType>;

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

}
