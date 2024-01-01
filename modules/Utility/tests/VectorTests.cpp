#include "Vector.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Vector Operations", "[Vector]")
{
    Vector a(1,2,3);
    Vector b(-1,-2, -4);

    REQUIRE(a+b == Vector(0, 0, -1));
    REQUIRE_FALSE(a+b == Vector(0, 0, 1));

    REQUIRE(a-b == Vector(2, 4, 7));

    REQUIRE(a/5 == Vector(0.2, 0.4, 0.6));

    REQUIRE(a*5 == Vector(5, 10, 15));

    REQUIRE(5*a == Vector(5, 10, 15));

    Vector c;
    REQUIRE(c.x == 0);
    REQUIRE(c.y == 0);
    REQUIRE(c.z == 0);

    c = a;
    REQUIRE(c.x == 1);
    REQUIRE(c.y == 2);
    REQUIRE(c.z == 3);

    c+=Vector();
    REQUIRE(c.x == 1);
    REQUIRE(c.y == 2);
    REQUIRE(c.z == 3);

    c+=Vector(1, 2, 3);
    REQUIRE(c.x == 2);
    REQUIRE(c.y == 4);
    REQUIRE(c.z == 6);

    c-=Vector(1, 2, 3);
    REQUIRE(c.x == 1);
    REQUIRE(c.y == 2);
    REQUIRE(c.z == 3);

    c*=0.5;
    REQUIRE(c.x == 0.5);
    REQUIRE(c.y == 1);
    REQUIRE(c.z == 1.5);

    c/=0.5;
    REQUIRE(c.x == 1);
    REQUIRE(c.y == 2);
    REQUIRE(c.z == 3);

    REQUIRE(a.dot(b) == -17);

    REQUIRE(a.cross(b) == Vector(-2, 1, 0));

    REQUIRE(a%b == Vector(-2, 1, 0));

    REQUIRE(a.abs() == sqrt(14));
    REQUIRE(b.abs() == sqrt(21));

    REQUIRE( normalise(a) == Vector(1/sqrt(14), 2/sqrt(14), 3/sqrt(14)) );

    REQUIRE(a.to_string() == "(1, 2, 3)");
}