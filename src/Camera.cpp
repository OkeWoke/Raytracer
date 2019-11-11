#include "Camera.h"

Camera::Camera()
{
    //ctor
}

Camera::Camera(int H_RES, int V_RES, double N, double H, double V, Vector& u, Vector &v, Vector& n, Vector& eye):
H_RES(H_RES),
V_RES(V_RES),
N(N),
H(H),
V(V),
u(u),
v(v),
n(n),
pos(eye)
{

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

    xml.FindElem("u");
    Vector::deserialize(xml.GetSubDoc(), cam.u);

    xml.FindElem("v");
    Vector::deserialize(xml.GetSubDoc(), cam.v);

    xml.FindElem("n");
    Vector::deserialize(xml.GetSubDoc(), cam.n);
}
