#include "raytracer.h"

#include "RendererBaseTypes.h"

#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>

#include "Image2D.h"


namespace LFOptics
{


const Real scene_epsilon = 0.05f;
const int default_maxraydepth = 4;



RayTracer::RayTracer(Scene* scene)
{
	m_Scene = scene;
	SetMaxRayDepth(default_maxraydepth);
}

RayTracer::~RayTracer()
{

}


bool RayTracer::SetMaxRayDepth( const Int raydepth )
{
	if ( raydepth < 0 )
		return false;
	this->maxRayDepth = raydepth;
	return true;
}


Vec4 RayTracer::ProcessRay(const Ray3& ray) const
{
	const Ray3 normalizedRay = Ray3( ray.origin, ray.direction.normalized() );
	const Vec4 tracedColor = CastRay(normalizedRay);
	if ( m_IsFinalColorOpaque )
	{
		return Vec4(tracedColor.xyz, 1);
	}
	else
	{
		tracedColor;
	}
}


Vec4 RayTracer::CastRay( const Ray3& ray, const Int depth ) const
{
	if (m_Scene == nullptr)
		return Vec4(0, 0, 0, 0);

	Vec3 point, N;
	Material material;

	if (depth > maxRayDepth || !m_Scene->Intersect( ray, point, N, material))
	{
		return m_Scene->background_color;
	}

	Vec4 reflect_color = Vec4(0, 0, 0, 0);
	Vec4 refract_color = Vec4(0, 0, 0, 0);

	if (material.is_reflective)
	{
		Vec3 reflect_dir = Reflect(ray.direction, N).normalized();
		Vec3 reflect_orig = dot(reflect_dir, N) < 0.0f ? point - N * scene_epsilon : point + N * scene_epsilon; // offset the original point to avoid occlusion by the object itself
		reflect_color = CastRay( Ray3(reflect_orig,reflect_dir), depth + 1);
	}

	if (material.is_refractive)
	{
		Vec3 refract_dir = Refract(ray.direction, N, material.refractive_index).normalized();
		Vec3 refract_orig = dot(refract_dir, N) < 0.0f ? point - N * scene_epsilon : point + N * scene_epsilon;
		refract_color = CastRay(Ray3(refract_orig, refract_dir), depth + 1);
	}

	if (material.is_ambient_only)
	{
		return material.diffuse_color;
	}
	else
	{
		Real diffuse_light_intensity = 0, specular_light_intensity = 0;
		for (auto light = m_Scene->lights.begin(); light != m_Scene->lights.end(); ++light)
		{
			Vec3 light_dir = (light->position - point).normalize();
			Real light_distance = (light->position - point).norm();

			Vec3 shadow_orig = dot(light_dir, N) < 0.0f ? point - N * scene_epsilon : point + N * scene_epsilon; // checking if the point lies in the shadow of the lights[i]
			Vec3 shadow_pt, shadow_N;
			Material tmpmaterial;
			if (m_Scene->Intersect( Ray3(shadow_orig, light_dir), shadow_pt, shadow_N, tmpmaterial) && (shadow_pt - shadow_orig).norm() < light_distance)
				continue;

			diffuse_light_intensity += light->intensity * std::max((Real)0, dot(light_dir, N));
			specular_light_intensity += pow(std::max((Real)0, -dot(Reflect(-light_dir, N), ray.direction)), material.specular_exponent) * light->intensity;
		}
		
		return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + Vec4(1, 1, 1, 1) * specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] + refract_color * material.albedo[3];
	}
}


Vec3 RayTracer::Reflect( const Vec3 &I, const Vec3 &N ) const
{
	return I - N * Real(2)*dot(I,N);
}


Vec3 RayTracer::Refract( const Vec3 &I, const Vec3 &N, const Real eta_t, const Real eta_i /*= 1.f */ ) const
{
	// Snell's law
	Real cosi = -std::max( Real(-1), std::min( Real(1), dot(I,N) ) );
	if ( cosi < 0 ) return Refract( I, -N, eta_i, eta_t ); // if the ray comes from the inside the object, swap the air and the media
	Real eta = eta_i / eta_t;
	Real k = 1 - eta * eta*(1 - cosi * cosi);
	return k < 0 ? Vec3(1,0,0) : I * eta + N * (eta*cosi - sqrt(k)); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
}


} // namespace LFOptics