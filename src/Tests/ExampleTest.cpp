#include <catch2/catch_all.hpp>
#include <string>

TEST_CASE("Example")
{
	REQUIRE(true);
}

TEST_CASE("Require Example")
{
	float value = 1.f;
	REQUIRE(value == 1.f);
}

// Doc for matchers(useful): https://github.com/catchorg/Catch2/blob/devel/docs/matchers.md
TEST_CASE("RequireThat float matchers examples")
{
	REQUIRE_THAT(0.6667f, Catch::Matchers::WithinAbs(0.666f, 0.001f));
	REQUIRE_THAT(std::sqrt(-1), Catch::Matchers::IsNaN());
}

TEST_CASE("RequireThat string matchers examples")
{
	std::string value = "HelloWorld";
	REQUIRE_THAT(value, Catch::Matchers::StartsWith("Hell"));
	REQUIRE_THAT(value, Catch::Matchers::EndsWith("orld"));
	REQUIRE_THAT(value, Catch::Matchers::ContainsSubstring("lloW"));
	REQUIRE_THAT(value, Catch::Matchers::Equals("HelloWorld"));
}