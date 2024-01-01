#include "stats.h"
#include <catch2/catch_test_macros.hpp>

extern Stats stats;

TEST_CASE("Stats Tests", "[Stats]")
{
    stats.clear();
    REQUIRE(stats.numPrimaryRays == 0);
    REQUIRE(stats.numRayTrianglesTests == 0);
    REQUIRE(stats.numRayTrianglesIsect == 0);

    stats.numPrimaryRays = 1;
    stats.numRayTrianglesTests = 2;
    stats.numRayTrianglesIsect = 3;

    REQUIRE(stats.numPrimaryRays == 1);
    REQUIRE(stats.numRayTrianglesTests == 2);
    REQUIRE(stats.numRayTrianglesIsect == 3);

    stats.clear();
    REQUIRE(stats.numPrimaryRays == 0);
    REQUIRE(stats.numRayTrianglesTests == 0);
    REQUIRE(stats.numRayTrianglesIsect == 0);
}