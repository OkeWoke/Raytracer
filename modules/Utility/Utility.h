#include <string>
#include <vector>
#include <stdint.h>
#include <atomic>
#include "Vector.h"
#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Utility {
	std::vector<std::string> split(const std::string& s, const std::string& delim);
	void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3);
	Vector snells_law(const Vector& incident_ray, const Vector& normal, double cos_angle, double n_1, double n_2);
	double schlick_fresnel(double cos_angle, double n_1, double n_2);

	Vector uniform_hemisphere(double u1, double u2, Vector& n);
	Vector cosine_weighted_hemisphere(double u1, double u2, const Vector& n);
	Vector uniform_sphere(double u1, double u2);
}

