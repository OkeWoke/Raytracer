#include "Camera.h"

Camera::Camera()
{
    double tmp[4][4] = {{0,-1,0,0},{1,0,0,0},{0,0,1,0},{0,0,0,1}};
    mat = Matrix(tmp);
}


void Camera::deserialize(std::string sub, Camera& cam)
{
    CMarkup xml(sub);

    xml.FindElem();
    cam.H = std::stod(xml.GetAttrib("w"));
    cam.V = std::stod(xml.GetAttrib("h"));
    cam.H_RES = std::stoi(xml.GetAttrib("H_Res"));
    cam.V_RES = std::stoi(xml.GetAttrib("V_Res"));

    cam.N = std::stod(xml.GetAttrib("fl"));
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), cam.pos);

    xml.FindElem("x_rot");

    Matrix tmp = cam.mat; //make sure its a carbon copy, not a reference.
    tmp = tmp * Matrix::rot_x(std::stod(xml.GetAttrib("angle")));
    //Vector::deserialize(xml.GetSubDoc(), cam.u);

    xml.FindElem("y_rot");
    tmp = tmp * Matrix::rot_y(std::stod(xml.GetAttrib("angle")));
    //Vector::deserialize(xml.GetSubDoc(), cam.v);

    xml.FindElem("z_rot");
    tmp = tmp * Matrix::rot_z(std::stod(xml.GetAttrib("angle")));
    //Vector::deserialize(xml.GetSubDoc(), cam.n);

    tmp = tmp * Matrix::translate(cam.pos);
    cam.n = Vector(tmp.ar[2][0], tmp.ar[2][1], tmp.ar[2][2]);
    cam.u = Vector(tmp.ar[1][0], tmp.ar[1][1], tmp.ar[1][2]);
    cam.v = Vector(tmp.ar[0][0], tmp.ar[0][1], tmp.ar[0][2]);
}

void Camera::update_camera(Matrix& tmp)
{
    this->n = Vector(tmp.ar[2][0], tmp.ar[2][1], tmp.ar[2][2]);
    this->u = Vector(tmp.ar[1][0], tmp.ar[1][1], tmp.ar[1][2]);
    this->v = Vector(tmp.ar[0][0], tmp.ar[0][1], tmp.ar[0][2]);
}
