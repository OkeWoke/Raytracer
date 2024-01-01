#include "Utility.h"
#include "Vector.h"
#include <vector>
#include <math.h>
#include <cassert>

namespace Utility {

    //below function is taken from smallpaint
    // given v1, set v2 and v3 so they form an orthonormal system
    // (we assume v1 is already normalized)
    void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3)
    {
        if (std::abs(v1.x) > std::abs(v1.y)) {
            // project to the y = 0 plane and construct a normalized orthogonal vector in this plane
            double invLen = 1.f / sqrtf(v1.x * v1.x + v1.z * v1.z);
            v2 = Vector(-v1.z * invLen, 0.0f, v1.x * invLen);
        }
        else {
            // project to the x = 0 plane and construct a normalized orthogonal vector in this plane
            double invLen = 1.0f / sqrtf(v1.y * v1.y + v1.z * v1.z);
            v2 = Vector(0.0f, v1.z * invLen, -v1.y * invLen);
        }
        v3 = v1 % v2;
    }

    std::vector<std::string> split(const std::string& s, const std::string& delim)
    {
        std::vector<std::string> splitted;
        std::string temp = s;

        while (temp.find(delim) != temp.npos)
        {
            int pos = temp.find(delim);
            splitted.push_back(temp.substr(0, pos));
            temp = temp.erase(0, pos + delim.length());
        }

        splitted.push_back(temp);
        return splitted;
    }

    Vector snells_law(const Vector& incident_ray, const Vector& normal, double cos_angle, double n_1, double n_2)
    {
        assert(incident_ray.abs() > 0.99 && incident_ray.abs() < 1.01);
        assert(normal.abs() > 0.99 && normal.abs() < 1.01);
        // Compute the refractive index ratio
        double n_ratio = n_1 / n_2;

        // Compute the cosines of the transmitted ray angle using Snell's Law
        double cos_transmitted_angle = std::sqrt(1.0 - n_ratio * n_ratio * (1.0 - cos_angle * cos_angle));

        // Compute the transmitted ray direction vector
        Vector transmitted_ray = n_ratio * incident_ray + (n_ratio * cos_angle - cos_transmitted_angle) * incident_ray;

        return normalise(transmitted_ray);
    }

    double schlick_fresnel(double cos_angle, double n_1, double n_2)
    // returns probabibility of reflection based on angle relative to normal. (0 to 90 degrees)
    {
        double R_0 = (n_1 - n_2) / (n_1 + n_2);
        R_0 = R_0 * R_0;

        double R_theta = R_0 + (1 - R_0) * pow((1 - cos_angle), 5);
        return R_theta;
    }

    Vector uniform_sphere(double u1, double u2)
    // returns a point on a sphere using two random numbers u1 and u2 between 0 and 1.
    // u1 used as the cosine of the polar angle, uniformly distributed.
    {
        const double r = sqrt(1.0 - u1 * u1);
        const double phi = 2 * M_PI * u2;
        Vector ray = Vector(cos(phi) * r, sin(phi) * r, u1);

        return ray;
    }

    Vector uniform_hemisphere(double u1, double u2, Vector& n)
    {
        Vector ray = uniform_sphere(u1, u2);
        if (ray.dot(n) < 0)
        {
            return -1 * ray;
        }
        return ray;
    }

    Vector cosine_weighted_hemisphere(double u1, double u2, const Vector& n)
        // taken from http://www.rorydriscoll.com/2009/01/07/better-sampling/
        // modified to ensure on the correct hemisphere
    {
        const double r = sqrt(u1);
        const double theta = 2 * M_PI * u2;

        const double x = r * cos(theta);
        const double y = r * sin(theta);
        const double z = 1 - std::sqrt(std::max(0.0, 1- u1));
        Vector trangent, bitangent;
        create_orthonormal_basis(n, trangent, bitangent);

        return x * trangent + y * bitangent + z * n;
    }
}
