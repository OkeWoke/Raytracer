#ifndef MATRIX_H
#define MATRIX_H
#include<Vector>

//4x4 matrix containing doubles, used for affine transformations
class Matrix
{
    public:
        Matrix();
        Matrix(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, double k, double l, double m, double n, double o, double p);

        //built in rotation matrices
        static Matrix rot_x(double theta);
        static Matrix rot_y(double theta);
        static Matrix rot_z(double theta);

        double a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;


    protected:

    private:
};

#endif // MATRIX_H
