#pragma once

#include "BaseTypes.h"
#include <vector>


namespace LFOptics
{


struct Light
{
	Light(const Vec3& p, const Real i) noexcept : position(p), intensity(i) {}
	Vec3 position;
	Real intensity;
};


struct Material
{
	Material( const Real r, const Vec4 &a, const Vec3 &color, const Real spec, const bool is_reflective, const bool is_refractive, const bool is_onlyambient = false ) noexcept
		: refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec), is_reflective(is_reflective), is_refractive(is_refractive), is_ambient_only(is_onlyambient) {}
	Material() noexcept : refractive_index(1), albedo(1,0,0,0), diffuse_color(), specular_exponent() {}
	Real refractive_index;
	Vec4 albedo;
	Vec4 diffuse_color;
	Real specular_exponent;
	bool is_reflective = false;
	bool is_refractive = false;
	bool is_ambient_only = false;
};


class Scene
{
public:
	virtual bool Intersect(const Ray3& ray, Vec3& hit, Vec3& N, Material& material) const = 0;
	std::vector<Light> lights;
	Vec3 background_color = Vec3(0,0,0);
};

} // namespace LFOptics