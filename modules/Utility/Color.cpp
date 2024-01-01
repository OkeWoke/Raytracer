#include "Color.h"
#include "Markup-w.h"
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

double Color::luminance()
{
    return r*0.299 +  g*0.587 +  b*0.114;
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

bool operator==(const Color& LHS, const Color& RHS)
{
    return (LHS.r == RHS.r && LHS.g == RHS.g && LHS.b == RHS.b);
}