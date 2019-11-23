#ifndef COLOR_H
#define COLOR_H
#include <string>
#include "ext/Markup.h"

class Color
{
    public:
        Color();
        Color(double r, double g, double b);
        double r;
        double g;
        double b;

        static void deserialize(std::string sub, Color& color);
};

Color operator+(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, double s);
Color operator*(double s, const Color& RHS);
Color operator/(const Color& LHS, double RHS);

#endif // COLOR_H
