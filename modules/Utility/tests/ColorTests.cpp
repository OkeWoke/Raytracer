#include "Color.h"
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Color Operations", "[Color]")
{
    Color a(1,2,3);
    Color b(-1,-2, -4);

    REQUIRE(a+b == Color(0, 0, -1));
    REQUIRE_FALSE(a+b == Color(0, 0, 1));


    REQUIRE(a/5 == Color(0.2, 0.4, 0.6));

    REQUIRE(a*5 == Color(5, 10, 15));

    REQUIRE(5*a == Color(5, 10, 15));

    Color c;
    REQUIRE(c.r == 0);
    REQUIRE(c.g == 0);
    REQUIRE(c.b == 0);
    REQUIRE(c.luminance() == 0);

    c = a;
    REQUIRE(c.r == 1);
    REQUIRE(c.g == 2);
    REQUIRE(c.b == 3);

    REQUIRE(c.luminance() == 1.815);

    REQUIRE(a.luminance() == 1.815);

    REQUIRE(a * b == Color(-1, -4, -12));
}