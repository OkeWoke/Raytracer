#include "Matrix.h"
#include <catch2/catch_test_macros.hpp>


bool compare (const Matrix& a, const Matrix& b)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j =0; j < 4; j++)
        {
            if (a.ar[i][j] != b.ar[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("Matrix Constructor", "[Matrix]")
{
    Matrix m = Matrix();
    REQUIRE(m.ar[0][0] == 1);
    REQUIRE(m.ar[0][1] == 0);
    REQUIRE(m.ar[0][2] == 0);
    REQUIRE(m.ar[0][3] == 0);
    REQUIRE(m.ar[1][0] == 0);
    REQUIRE(m.ar[1][1] == 1);
    REQUIRE(m.ar[1][2] == 0);
    REQUIRE(m.ar[1][3] == 0);
    REQUIRE(m.ar[2][0] == 0);
    REQUIRE(m.ar[2][1] == 0);
    REQUIRE(m.ar[2][2] == 1);
    REQUIRE(m.ar[2][3] == 0);
    REQUIRE(m.ar[3][0] == 0);
    REQUIRE(m.ar[3][1] == 0);
    REQUIRE(m.ar[3][2] == 0);
    REQUIRE(m.ar[3][3] == 1);

    Matrix other = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
    REQUIRE(other.ar[0][0] == 1);
    REQUIRE(other.ar[0][1] == 2);
    REQUIRE(other.ar[0][2] == 3);
    REQUIRE(other.ar[0][3] == 4);
    REQUIRE(other.ar[1][0] == 5);
    REQUIRE(other.ar[1][1] == 6);
    REQUIRE(other.ar[1][2] == 7);
    REQUIRE(other.ar[1][3] == 8);
    REQUIRE(other.ar[2][0] == 9);
    REQUIRE(other.ar[2][1] == 10);
    REQUIRE(other.ar[2][2] == 11);
    REQUIRE(other.ar[2][3] == 12);
    REQUIRE(other.ar[3][0] == 13);
    REQUIRE(other.ar[3][1] == 14);
    REQUIRE(other.ar[3][2] == 15);
    REQUIRE(other.ar[3][3] == 16);

    double arr[4][4] = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};

    Matrix other2 = Matrix(arr);
    REQUIRE(other2.ar[0][0] == 1);
    REQUIRE(other2.ar[0][1] == 2);
    REQUIRE(other2.ar[0][2] == 3);
    REQUIRE(other2.ar[0][3] == 4);
    REQUIRE(other2.ar[1][0] == 5);
    REQUIRE(other2.ar[1][1] == 6);
    REQUIRE(other2.ar[1][2] == 7);
    REQUIRE(other2.ar[1][3] == 8);
    REQUIRE(other2.ar[2][0] == 9);
    REQUIRE(other2.ar[2][1] == 10);
    REQUIRE(other2.ar[2][2] == 11);
    REQUIRE(other2.ar[2][3] == 12);
    REQUIRE(other2.ar[3][0] == 13);
    REQUIRE(other2.ar[3][1] == 14);
    REQUIRE(other2.ar[3][2] == 15);
    REQUIRE(other2.ar[3][3] == 16);
}


TEST_CASE("Matrix Inverse", "[Matrix]")
{
    Matrix m = Matrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    Matrix m_inv = Matrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    Matrix inv = m.inverse();
    compare(inv, m_inv);

    Matrix m2 = Matrix(2,0,0,0,0,3,0,0,0,0,4,0,0,0,0,5);
    Matrix inv2 = m2.inverse();
    Matrix m2_inv = Matrix(0.5,0,0,0,0,1.0/3.0,0,0,0,0,0.25,0,0,0,0,0.2);
    compare(inv2, m2_inv);
}

TEST_CASE("Matrix String", "[Matrix]")
{
    Matrix m = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);

    std::string s = m.to_string();
    std::string expected = "1 2 3 4\n5 6 7 8\n9 10 11 12\n13 14 15 16\n";
    REQUIRE(s == expected);
}

TEST_CASE("Matrix Translate", "[Matrix]")
{
    Vector v = Vector(1,2,3);
    Matrix m = Matrix::translate(v);
    Matrix expected = Matrix(1,0,0,1,0,1,0,2,0,0,1,3,0,0,0,1);
    compare(m, expected);

    Vector v2 = Vector(3,-4, 7);
    Matrix m2 = Matrix::translate(v2);
    Matrix expected2 = Matrix(1,0,0,3,0,1,0,-4,0,0,1,7,0,0,0,1);
    compare(m2, expected2);
}

TEST_CASE("Matrix rotation", "[Matrix]")
{
    Matrix m = Matrix::rot_x(90);
    Matrix expected = Matrix(1,0,0,0,0,0,-1,0,0,1,0,0,0,0,0,1);
    compare(m, expected);

    Matrix m2 = Matrix::rot_y(90);
    Matrix expected2 = Matrix(0,0,1,0,0,1,0,0,-1,0,0,0,0,0,0,1);
    compare(m2, expected2);

    Matrix m3 = Matrix::rot_z(90);
    Matrix expected3 = Matrix(0,-1,0,0,1,0,0,0,0,0,1,0,0,0,0,1);
    compare(m3, expected3);


    Matrix m4 = Matrix::rot_x(180);
    Matrix expected4 = Matrix(1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1);
    compare(m4, expected4);

    Matrix m5 = Matrix::rot_y(180);
    Matrix expected5 = Matrix(-1,0,0,0,0,1,0,0,0,0,-1,0,0,0,0,1);
    compare(m5, expected5);

    Matrix m6 = Matrix::rot_z(180);
    Matrix expected6 = Matrix(-1,0,0,0,0,-1,0,0,0,0,1,0,0,0,0,1);
    compare(m6, expected6);

    Matrix m7 = Matrix::rot_x(45);
    Matrix expected7 = Matrix(1,0,0,0,0,0.707107,-0.707107,0,0,0.707107,0.707107,0,0,0,0,1);
    compare(m7, expected7);

    Matrix m8 = Matrix::rot_y(45);
    Matrix expected8 = Matrix(0.707107,0,-0.707107,0,0,1,0,0,0.707107,0,0.707107,0,0,0,0,1);
    compare(m8, expected8);

    Matrix m9 = Matrix::rot_z(45);
    Matrix expected9 = Matrix(0.707107,-0.707107,0,0,0.707107,0.707107,0,0,0,0,1,0,0,0,0,1);
    compare(m9, expected9);
}

TEST_CASE("Matrix Scale", "[Matrix]")
{
    Matrix m = Matrix::scale(2);
    Matrix expected = Matrix(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,1);
    compare(m, expected);

    Matrix m2 = Matrix::scale(0.5);
    Matrix expected2 = Matrix(0.5,0,0,0,0,0.5,0,0,0,0,0.5,0,0,0,0,1);
    compare(m2, expected2);
}

TEST_CASE("Matrix mult_vec", "[Matrix]")
{
    Matrix m = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,0,0,0,1);
    Vector v = Vector(1,2,3);
    Vector expected = Vector(18,46,74);
    Vector result = m.mult_vec(v, 1);
    REQUIRE(result.x == expected.x);
    REQUIRE(result.y == expected.y);
    REQUIRE(result.z == expected.z);

    Matrix m2 = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,0,0,0,1);
    Vector v2 = Vector(1,2,3);
    Vector expected2 = Vector(18,46,74);
    Vector result2 = m2.mult_vec(v2, 1);
    REQUIRE(result2.x == expected2.x);
    REQUIRE(result2.y == expected2.y);
    REQUIRE(result2.z == expected2.z);
}

TEST_CASE("Matrix mult", "[Matrix]")
{
    Matrix m = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,0,0,0,1);
    Matrix m2 = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,0,0,0,1);
    Matrix expected = Matrix(38,44,50,56,83,98,113,128,128,152,176,200,0,0,0,1);
    Matrix result = m * m2;
    compare(result, expected);

    Matrix m3 = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,0,0,0,1);
    Matrix m4 = Matrix(1,2,3,4,5,6,7,8,9,10,11,12,0,0,0,1);
    Matrix expected2 = Matrix(38,44,50,56,83,98,113,128,128,152,176,200,0,0,0,1);
    Matrix result2 = m3 * m4;
    compare(result2, expected2);
}