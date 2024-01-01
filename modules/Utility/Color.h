#pragma once

#include <string>



class Color
{
    public:
        Color();
        Color(double r, double g, double b);
        double r;
        double g;
        double b;

        double luminance();
};

Color operator+(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, double s);
Color operator*(double s, const Color& RHS);
Color operator/(const Color& LHS, double RHS);

