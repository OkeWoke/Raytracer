#ifndef LIGHT_H
#define LIGHT_H
#include "Vector.h"
#include "Color.h"
class Light
{
    public:
        Light();
        Light(Vector pos, Color a, Color d, Color s);

        Vector position;
        Color ambient;
        Color diffuse;
        Color specular;

    protected:

    private:
};

#endif // LIGHT_H
