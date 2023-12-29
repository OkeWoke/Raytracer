#pragma once

#include <cmath>
#include <sstream>
#include <string>
#include <iostream>

#include "Vector.h"

//4x4 matrix containing doubles, used for affine transformations.
class Matrix
{
    public:
        //constructors
        Matrix();
        Matrix(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, double k, double l, double m, double n, double o, double p);
        Matrix(double mat[4][4]);

        //built in rotation matrices
        static Matrix rot_x(double theta);
        static Matrix rot_y(double theta);
        static Matrix rot_z(double theta);
        static Matrix scale(double factor);

        //computations
        Vector mult_vec(Vector lhs, double w) const;
        static Matrix translate(Vector vec);
        Matrix inverse();

        //data
        double a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
        double ar[4][4];

        //utility
        std::string to_string();
};

Matrix operator* (const Matrix& L, const Matrix& R);
