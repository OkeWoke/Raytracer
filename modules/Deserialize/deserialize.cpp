#include "deserialize.h"

MeshConfig deserializeMesh(std::basic_string<char> xmlStr)
{
    MeshConfig meshConfig;
    CMarkup xml(xmlStr);

    xml.FindElem();

    meshConfig.filename = xml.GetAttrib("filename");
    meshConfig.shininess = std::stod(xml.GetAttrib("shininess"));
    meshConfig.reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    meshConfig.brdf = std::stod(xml.GetAttrib("brdf"));
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), meshConfig.position);
    meshConfig.mat = meshConfig.mat * Matrix::translate(meshConfig.position);

    xml.FindElem("x_rot");
    meshConfig.mat = meshConfig.mat * Matrix::rot_x(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("y_rot");
    meshConfig.mat = meshConfig.mat * Matrix::rot_y(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("z_rot");
    meshConfig.mat = meshConfig.mat * Matrix::rot_z(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("scale");
    meshConfig.mat = meshConfig.mat * Matrix::scale(std::stod(xml.GetAttrib("factor")));

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), meshConfig.color);

    xml.FindElem("texture");
    meshConfig.texture_filename = xml.GetAttrib("filename");

    return meshConfig;
}