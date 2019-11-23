#include "Light.h"

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

void Light::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), position);

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);
}

