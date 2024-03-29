#pragma once

#include <string>
#include <sstream>
#include <math.h>



class Vector
{
    public:
        Vector();
        Vector(double x, double y, double z);
        Vector(const Vector& RHS);

        Vector& operator=(const Vector& RHS);
        Vector& operator+=(const Vector& RHS);
        Vector& operator-=(const Vector& RHS);
        Vector& operator*=(double RHS);
        Vector& operator/=(double RHS);

        double abs() const;
        double dot (Vector V) const;

        std::string to_string() const;

        double x;
        double y;
        double z;

        Vector cross(const Vector& vec);
};

bool operator==(const Vector& LHS, const Vector& RHS);
Vector operator+(const Vector& LHS, const Vector& RHS);
Vector operator-(const Vector& LHS, const Vector& RHS);
Vector operator*(const Vector& LHS, double RHS);
Vector operator*(double LHS, const Vector& RHS);
Vector operator/ (const Vector& LHS, double RHS);
Vector operator%(const Vector& LHS, const Vector& RHS);
Vector normalise(const Vector& vec);
