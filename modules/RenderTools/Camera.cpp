#include "Camera.h"
#include "Markup-w.h"

Camera::Camera()
{
    double tmp[4][4] = {{0,1,0,0},{1,0,0,0},{0,0,1,0},{0,0,0,1}};
    mat = Matrix(tmp);
}

void Camera::update_camera(Matrix& tmp)
{
    this->n = Vector(tmp.ar[2][0], tmp.ar[2][1], tmp.ar[2][2]);
    this->u = Vector(tmp.ar[0][0], tmp.ar[0][1], tmp.ar[0][2]);
    this->v = -1*Vector(tmp.ar[1][0], tmp.ar[1][1], tmp.ar[1][2]); // flip this axis such that -x is leftward.
    this->pos = Vector(tmp.ar[0][3], tmp.ar[1][3], tmp.ar[2][3]);
    this->mat = tmp;
}
