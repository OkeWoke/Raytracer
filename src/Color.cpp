#include "Color.h"

Color::Color():
r(0),
g(0),
b(0)
{

}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color operator+(const Color& LHS, const Color& RHS)
{
    return Color(LHS.r + RHS.r, LHS.g + RHS.g, LHS.b + RHS.b);
}

Color operator*(const Color& LHS, const Color& RHS)
{
    return Color(LHS.r * RHS.r, LHS.g * RHS.g, LHS.b * RHS.b);
}

Color operator*(const Color& LHS, double s)
{
    return Color(LHS.r * s, LHS.g * s, LHS.b * s);
}

Color operator*(double s, const Color& RHS)
{
    return RHS*s;
}

Color operator/(const Color& LHS, double RHS)
{
    return Color(LHS.r/RHS, LHS.g/RHS, LHS.b/RHS);
}
