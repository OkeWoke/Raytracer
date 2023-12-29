#include "Utility.h"
#include "Vector.h"
#include <vector>
#include <math.h>


namespace Utility {

    //below function is taken from smallpaint
    // given v1, set v2 and v3 so they form an orthonormal system
    // (we assume v1 is already normalized)
    void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3) {
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
        //assume the two rays are normalised, thus dot product returns the cosine of them.
        //takes cos_angle to avoid doing a redundant dot product.
    {
        Vector ray_hor = incident_ray - cos_angle * normal;
        double sin_theta_2 = ray_hor.abs() * n_1 / n_2;
        Vector refr_ray = normalise(-1 * normal + normalise(ray_hor) * sin_theta_2);
        return refr_ray;
    }

    double schlick_fresnel(double cos_angle, double n_1, double n_2)
        //returns probabibility of reflection based on angle relative to normal. (0 to 90 degrees)
    {
        double R_0 = (n_1 - n_2) / (n_1 + n_2);
        R_0 = R_0 * R_0;

        double R_theta = R_0 + (1 - R_0) * pow((1 - cos_angle), 5);
        return R_theta;
    }

    Vector uniform_sphere(double u1, double u2)
    {
        const double r = sqrt(1.0 - u1 * u1);
        const double phi = 2 * M_PI * u2;
        Vector ray = Vector(cos(phi) * r, sin(phi) * r, u1);

        return ray;
    }

    Vector uniform_hemisphere(double u1, double u2, Vector& n) {
        const double r = sqrt(1.0 - u1 * u1);
        const double phi = 2 * M_PI * u2;
        Vector ray = Vector(cos(phi) * r, sin(phi) * r, u1);
        if (ray.dot(n) < 0)
        {
            return -1 * ray;
        }
        return ray;
    }

    Vector cosine_weighted_hemisphere(double u1, double u2, const Vector& n)
        // taken from http://www.rorydriscoll.com/2009/01/07/better-sampling/
        //modified to ensure on the correct hemisphere
    {
        const double r = sqrt(u1);
        const double theta = 2 * M_PI * u2;

        const double x = r * cos(theta);
        const double y = r * sin(theta);
        const double z = 1 - x * x - y * y;

        Vector ray = Vector(x, y, z);
        if (ray.dot(n) < 0)
        {
            return -1 * ray;
        }
        return ray;
    }
}
