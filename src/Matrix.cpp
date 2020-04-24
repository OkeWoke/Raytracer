#include "Matrix.h"

Matrix::Matrix() : Matrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
{

}

Matrix::Matrix(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, double k, double l, double m, double n, double o, double p):
a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h), i(i), j(j), k(k), l(l), m(m), n(n), o(o), p(p)
{
    //2d array copy for inverse calc
    ar[0][0] = a;
    ar[0][1] = b;
    ar[0][2] = c;
    ar[0][3] = d;

    ar[1][0] = e;
    ar[1][1] = f;
    ar[1][2] = g;
    ar[1][3] = h;

    ar[2][0] = i;
    ar[2][1] = j;
    ar[2][2] = k;
    ar[2][3] = l;

    ar[3][0] = m;
    ar[3][1] = n;
    ar[3][2] = o;
    ar[3][3] = p;
}

Matrix::Matrix(double mat[4][4]): Matrix(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3])
{
}

Matrix Matrix::inverse()
{
    double aug[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

    //duping current matrix
    double tmp[4][4];
    for(int i =0; i < 4; i++)
    {
        for(int j =0; j < 4; j++)
        {
            tmp[i][j] = ar[i][j];
        }
    }

    auto swap_rows = [this](double mat[4][4], int r1, int r2)
    //swaps rows r1 with r2
    {
        for (int i=0; i < 4; i++)
        {
            double temp;
            temp = mat[r2][i];
            mat[r2][i] = mat[r1][i];
            mat[r1][i] = temp;
        }
    };

    auto mult_row = [this](double mat[4][4], double scalar, int row)
    //multiplies specified row by scalar
    {
        for (int i =0; i < 4; i++)
        {
            mat[row][i] = mat[row][i]*scalar;
        }
    };

    auto sub_row = [this](double mat[4][4], double scalar, int r1, int r2)
    //subtracts a multiple of a row from another.
    // r2 = r2 - r1*scalar
    {
        for (int i = 0; i < 4; i++)
        {
            mat[r2][i] = mat[r2][i] - mat[r1][i]*scalar;
        }
    };

    auto swap_largest_in_col = [this, swap_rows](double mat[4][4], double aug[4][4], int col)
    //swaps out rows such that largest value in column col, is at row r
    {
        int max_r = col;
        for(int i= col; i< 4;i++)
        {
            if(mat[i][col] > mat[max_r][col])
            {
                max_r = i;
            }
        }
        swap_rows(mat, col, max_r);
        swap_rows(aug, col, max_r);
    };

    auto scale_largest_row = [this, mult_row](double mat[4][4], double aug[4][4], int col)
    {
        double divisor= 1/ mat[col][col];
        mult_row(mat, divisor, col);
        mult_row(aug, divisor, col);
    };

    auto sub_rest_rows = [this, sub_row](double mat[4][4], double aug[4][4], int col)
    //subtracts from all rows except row col, subtraction results in 0.
    {
        for(int i=0; i<4; i++)
        {
            if(i!=col)
            {
                double val = mat[i][col];
                sub_row(mat, mat[i][col], col, i);
                sub_row(aug, val, col, i);
            }
        }
    };

    auto mat_string = [this](double mat[4][4])
    {
        std::ostringstream oss;
        oss << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << " " << mat[0][3] << std::endl;
        oss << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << " " << mat[1][3] << std::endl;
        oss << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << " " << mat[2][3] << std::endl;
        oss << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << std::endl;
        return oss.str();
    };

    for (int i=0; i<4; i++)
    {
        swap_largest_in_col(tmp, aug, i);
        scale_largest_row(tmp, aug,i);
        sub_rest_rows(tmp, aug, i);
    }

    return Matrix(aug);
}


std::string Matrix::to_string()
{
    std::ostringstream oss;
	oss << ar[0][0] << " " << ar[0][1] << " " << ar[0][2] << " " << ar[0][3] << std::endl;
	oss << ar[1][0] << " " << ar[1][1] << " " << ar[1][2] << " " << ar[1][3] << std::endl;
	oss << ar[2][0] << " " << ar[2][1] << " " << ar[2][2] << " " << ar[2][3] << std::endl;
	oss << ar[3][0] << " " << ar[3][1] << " " << ar[3][2] << " " << ar[3][3] << std::endl;
	return oss.str();
}

Matrix Matrix::translate(Vector vec)
{
    return Matrix(1, 0, 0, vec.x, 0, 1, 0, vec.y, 0, 0, 1, vec.z, 0, 0 ,0 ,1);
}

Matrix Matrix::rot_x(double theta)
//takes an angle in degrees, returns rotation around x axis matrix
{
    theta = PI * theta/180;
    double s = sin(theta);
    double c = cos(theta);
    return Matrix(1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1);
}

Matrix Matrix::rot_y(double theta)
//takes an angle in degrees, returns rotation around y axis matrix
{
    theta = PI * theta/180;
    double s = sin(theta);
    double c = cos(theta);
    return Matrix(c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1);
}

Matrix Matrix::rot_z(double theta)
//takes an angle in degrees, returns rotation around z axis matrix
{
    theta = PI * theta/180;
    double s = sin(theta);
    double c = cos(theta);
    return Matrix(c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

Matrix Matrix::scale(double factor)
{
    return Matrix(factor, 0, 0, 0, 0, factor, 0, 0, 0, 0, factor, 0, 0, 0, 0, factor);
}

Vector Matrix::mult_vec(Vector lhs, double w)
//transform a 3 dim vec with 4th homogenous coordinate w
{
    return Vector(
                  a * lhs.x + b * lhs.y + c * lhs.z + d * w,
                  e * lhs.x + f * lhs.y + g * lhs.z + h * w,
                  i * lhs.x + j * lhs.y + k * lhs.z + l * w
                  );
}

Matrix operator* (const Matrix& L, const Matrix& R)
{
    double elems[16];
    //this method is apparently faster than iterating over a 2d array so yes I typed this all out.
    elems[0] = L.a * R.a + L.b * R.e + L.c * R.i + L.d * R.m;
    elems[1] = L.a * R.b + L.b * R.f + L.c * R.j + L.d * R.n;
    elems[2] = L.a * R.c + L.b * R.g + L.c * R.k + L.d * R.o;
    elems[3] = L.a * R.d + L.b * R.h + L.c * R.l + L.d * R.p;

    elems[4] = L.e * R.a + L.f * R.e + L.g * R.i + L.h * R.m;
    elems[5] = L.e * R.b + L.f * R.f + L.g * R.j + L.h * R.n;
    elems[6] = L.e * R.c + L.f * R.g + L.g * R.k + L.h * R.o;
    elems[7] = L.e * R.d + L.f * R.h + L.g * R.l + L.h * R.p;

    elems[8] = L.i * R.a + L.j * R.e + L.k * R.i + L.l * R.m;
    elems[9] = L.i * R.b + L.j * R.f + L.k * R.j + L.l * R.n;
    elems[10] = L.i * R.c + L.j * R.g + L.k * R.k + L.l * R.o;
    elems[11] = L.i * R.d + L.j * R.h + L.k * R.l + L.l * R.p;

    elems[12] = L.m * R.a + L.n * R.e + L.o * R.i + L.p * R.m;
    elems[13] = L.m * R.b + L.n * R.f + L.o * R.j + L.p * R.n;
    elems[14] = L.m * R.c + L.n * R.g + L.o * R.k + L.p * R.o;
    elems[15] = L.m * R.d + L.n * R.h + L.o * R.l + L.p * R.p;

    return Matrix(elems[0], elems[1], elems[2], elems[3], elems[4], elems[5], elems[6], elems[7], elems[8], elems[9], elems[10], elems[11], elems[12], elems[13], elems[14], elems[15]);
}
