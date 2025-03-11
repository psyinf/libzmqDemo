#include <sampleLib/core/lib.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple", "[SampleLibTests]")
{
    REQUIRE("Hello, World!" == sampleLib::hello_world());
}
