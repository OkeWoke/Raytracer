#include "Light.h"
#include "Markup-w.h"

Light::Light():
position(Vector()),
color(Color())
{
}

Light::Light(Vector pos, Color c):
position(pos),
color(c)
{
}

