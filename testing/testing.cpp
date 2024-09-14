
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#define CATCH_CONFIG_MAIN

TEST_CASE("Addition", "[math]") {
    REQUIRE(1 + 1 == 2);
}

TEST_CASE("Substraction", "[math]") {
    REQUIRE(1 - 1 == 0);
}