#pragma once

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
