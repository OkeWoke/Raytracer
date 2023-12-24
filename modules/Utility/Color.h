#pragma once

#include <string>

#include "Markup-w.h"

class Color
{
    public:
        Color();
        Color(double r, double g, double b);
        double r;
        double g;
        double b;

        static void deserialize(std::string sub, Color& color);
        double luminance();
};

Color operator+(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, double s);
Color operator*(double s, const Color& RHS);
Color operator/(const Color& LHS, double RHS);

