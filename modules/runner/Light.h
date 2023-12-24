#ifndef LIGHT_H
#define LIGHT_H
#include "Vector.h"
#include "Color.h"
class Light
{
    public:
        Light();
        Light(Vector pos, Color c);

        Vector position;
        Color color;

        void deserialize(std::string strSubDoc);
};

#endif // LIGHT_H
