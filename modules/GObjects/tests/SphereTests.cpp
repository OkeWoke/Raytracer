#include "Sphere.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Sphere Tests", "[Sphere]")
{
    Vector a(1,2,3);
    Vector b(-1,-2, -4);

    REQUIRE(a+b == Vector(0, 0, -1));
    REQUIRE_FALSE(a+b == Vector(0, 0, 1));

    REQUIRE(a-b == Vector(2, 4, 7));

    REQUIRE(a/5 == Vector(0.2, 0.4, 0.6));

    REQUIRE(a*5 == Vector(5, 10, 15));
}