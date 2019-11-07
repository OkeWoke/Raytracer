#include "Light.h"

Light::Light():
position(Vector()),
ambient(Color()),
diffuse(Color()),
specular(Color())
{

}

Light::Light(Vector pos, Color a, Color d, Color s)
{
    position = pos;
    ambient = a;
    diffuse = d;
    specular = s;
}

