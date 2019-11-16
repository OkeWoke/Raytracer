#include "Vector.h"
#include<iostream>
Vector::Vector():
x(0),
y(0),
z(0)
{
}

Vector::Vector(double x, double y, double z):
x(x),
y(y),
z(z)
{
}

Vector::Vector(const Vector& RHS)
{
    this->x = RHS.x;
    this->y = RHS.y;
    this->z = RHS.z;
}

Vector& Vector::operator=(const Vector& RHS)
{
    this->x=RHS.x;
    this->y=RHS.y;
    this->z=RHS.z;

    return *this;
}

Vector& Vector::operator+=(const Vector& RHS)
{
    this->x+=RHS.x;
    this->y+=RHS.y;
    this->z+=RHS.z;

    return *this;
}

Vector& Vector::operator-=(const Vector& RHS)
{
    this->x-=RHS.x;
    this->y-=RHS.y;
    this->z-=RHS.z;

    return *this;
}

Vector& Vector::operator*=(double RHS)
{
    this->x*=RHS;
    this->y*=RHS;
    this->z*=RHS;

    return *this;
}

Vector& Vector::operator/=(double RHS)
{
    this->x/=RHS;
    this->y/=RHS;
    this->z/=RHS;

    return *this;
}

double Vector::dot(Vector v) const
{
    return this->x*v.x + this->y*v.y + this->z*v.z;
}

Vector operator+(const Vector& LHS, const Vector& RHS)
{
    Vector result = LHS;
    result+=RHS;

    return result;
}

Vector operator-(const Vector& LHS, const Vector& RHS)
{
    Vector result = LHS;
    result-=RHS;

    return result;
}

Vector operator*(const Vector& LHS, double RHS)
{
    Vector result = LHS;
    result*=RHS;

    return result;
}

Vector operator/(const Vector& LHS, double RHS)
{
    Vector result = LHS;
    result/=RHS;

    return result;
}

Vector operator*(double LHS, const Vector& RHS)
{
    Vector result = RHS;
    result*=LHS;

    return result;
}

bool operator==(const Vector& LHS, const Vector& RHS)
{
    return (LHS.x==RHS.x && LHS.y==RHS.y && LHS.z==RHS.z);
}

double Vector::abs() const
{
    return sqrt(x*x + y*y + z*z);
}

Vector normalise(const Vector& vec)
{
    return (vec/vec.abs());
}

Vector Vector::cross(const Vector& vec)
{
    return Vector(this->y*vec.z - this->z*vec.y, this->z*vec.x - this->x*vec.z, this->x*vec.y - this->y*vec.x);
}

std::string Vector::to_string() const
{
	std::ostringstream oss;
	oss << "(" << this->x << ", " << this->y <<", " << this->z << ")";
	return oss.str();
}

void Vector::deserialize(std::string sub, Vector& vec)
 {
    CMarkup xml(sub);
    xml.FindElem();
    vec.x = std::stod(xml.GetAttrib("x"));
    vec.y = std::stod(xml.GetAttrib("y"));
    vec.z = std::stod(xml.GetAttrib("z"));
 }


