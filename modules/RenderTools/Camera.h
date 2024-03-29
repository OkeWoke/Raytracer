#pragma once

#include "Vector.h"
#include "Matrix.h"
#include <string>

class Camera
{
    public:
        Camera();
        Camera(int H_RES, int V_RES, double N, double H, double V, Vector& u, Vector &v, Vector& n, Vector& eye);
        int H_RES, V_RES;
        double N, H, V, aperture, focus_dist;
        Vector u,v,n,pos;
        Matrix mat;

        void update_camera(Matrix& tmp);
};

