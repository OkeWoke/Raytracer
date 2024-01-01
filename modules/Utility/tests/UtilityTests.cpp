#include "Utility.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

TEST_CASE("Utility::split", "[Utility]")
{
    std::string str = "This is a test";
    std::vector<std::string> result = Utility::split(str, " ");
    REQUIRE(result.size() == 4);
    REQUIRE(result[0] == "This");
    REQUIRE(result[1] == "is");
    REQUIRE(result[2] == "a");
    REQUIRE(result[3] == "test");

    str = "1/2/3";
    result = Utility::split(str, "/");
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == "1");
    REQUIRE(result[1] == "2");
    REQUIRE(result[2] == "3");
}

TEST_CASE("Utility::create_orthonormal_basis", "[Utility]")
{
    Vector v1 = Vector(1,0,0);
    Vector v2 = Vector(0,1,0);
    Vector v3 = Vector(0,0,1);
    Utility::create_orthonormal_basis(v1, v2, v3);
    REQUIRE(v1 == Vector(1,0,0));
    REQUIRE(v2 == Vector(-0,0,1));
    REQUIRE(v3 == Vector(0,-1,0));

    v1 = Vector(1,1,1);
    v2 = Vector(1,1,1);
    v3 = Vector(1,1,1);
    Utility::create_orthonormal_basis(v1, v2, v3);
    REQUIRE(v1 == Vector(1,1,1));
    REQUIRE(v2 == Vector(0,0.70710676908493042,-0.70710676908493042));
    REQUIRE(v3 == Vector(-1.4142135381698608,0.70710676908493042,0.70710676908493042));

}

TEST_CASE("Utility::snells_law", "[Utility]")
{
    Vector incident_ray = normalise(Vector(1, 1, 0)); // 45 deg angle
    Vector normal = Vector(0, -1, 0);
    double cos_angle = cos(M_PI / 4); // 45 deg angle
    double n_1 = 1;
    double n_2 = 2;
    Vector refr_ray = Utility::snells_law(incident_ray, normal, cos_angle, n_1, n_2);

    REQUIRE(refr_ray == Vector(0.35355339059327379, 0.93541434669348533, 0));

    // TODO: Add more cases here
}

TEST_CASE("Utility::schlick_fresnel", "[Utility]")
{
    double cos_angle = cos(M_PI / 4); // 45 deg angle
    double n_1 = 1;
    double n_2 = 2;
    double fresnel = Utility::schlick_fresnel(cos_angle, n_1, n_2);

    REQUIRE(fresnel == Catch::Approx(0.1130271047));
}

TEST_CASE("Utility::uniform_hemisphere", "[Utility]")
{
    Vector n = Vector(0, 1, 0);
    Vector ray = Utility::uniform_hemisphere(0.5, 0.5, n);

    REQUIRE(ray == Vector(-0.8660254037844386, 1.0605752387249068E-16, 0.5));
}

TEST_CASE("Utility::uniform_sphere", "[Utility]")
{
    Vector ray = Utility::uniform_sphere(0.5, 0.5);

    REQUIRE(ray == Vector(-0.8660254037844386, 1.0605752387249068E-16, 0.5));
}

TEST_CASE("Utility::cosine_weighted_hemisphere", "[Utility]")
{
    Vector n = Vector(0, 1, 0);
    Vector ray = Utility::cosine_weighted_hemisphere(0.5, 0.5, n);

    REQUIRE(ray == Vector(-8.6595605623549341E-17, 0.29289321881345243, 0.70710678118654757));
}