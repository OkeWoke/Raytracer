#ifndef COLOR_H
#define COLOR_H


class Color
{
    public:
        Color();
        Color(double r, double g, double b);
        double r;
        double g;
        double b;

    protected:

    private:
};


Color operator+(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, const Color& RHS);
Color operator*(const Color& LHS, double s);
Color operator*(double s, const Color& RHS);
Color operator/(const Color& LHS, double RHS);
#endif // COLOR_H
