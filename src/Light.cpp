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

void Light::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), position);

    xml.FindElem("ambient");
    Color::deserialize(xml.GetSubDoc(), ambient);

    xml.FindElem("diffuse");
    Color::deserialize(xml.GetSubDoc(), diffuse);

    xml.FindElem("specular");
    Color::deserialize(xml.GetSubDoc(), specular);
}

