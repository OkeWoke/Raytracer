#ifndef MATRIX_H
#define MATRIX_H
#include "Vector.h"
#include<cmath>
#include <sstream>
#include <string>
#include <iostream>
#define PI 3.14159265

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

        //computations
        static Matrix translate(Vector vec);
        Matrix inverse();

        //data
        double a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
        double ar[4][4];

        //utility
        std::string to_string();
};

Matrix operator* (const Matrix& L, const Matrix& R);
#endif // MATRIX_H
