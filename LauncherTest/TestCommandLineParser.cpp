#include "Launcher/Sources/CommandLineParser.hpp"

#include <catch2/catch_template_test_macros.hpp>

TEMPLATE_TEST_CASE("Using the Command Line Parser", "[launcher]", char)
{
    using Parser  = CommandLineParser<TestType>;
    using Results = CommandLineParseResults<TestType>;

    Parser parser;

    SECTION("With no args, the parameters are empty")
    {
        Results const params = parser.parse(nullptr);
        REQUIRE(params.isEmpty());
    }
}
