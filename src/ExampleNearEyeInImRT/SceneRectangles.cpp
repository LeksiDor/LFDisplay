#include "SceneRectangles.h"

#include <fstream>
#include <limits>

namespace LFOptics
{

const Real epsilon = std::numeric_limits<Real>::epsilon();


SceneRectangles::SceneRectangles(const Affine3& positioning)
{
	SetPositioning(positioning);

	//background_color = Vec3(1,1,1);
	background_color = Vec3(0.2,0.7,0.8);
}


bool SceneRectangles::Intersect(const Ray3& ray, Vec3& hit, Vec3& N, Material& material) const
{
#if 1
	Ray3 ray_local = m_GlobalToLocal * ray;
	ray_local.direction.normalize();
	Vec3 hit_local;
	Vec3 N_local;
	const bool result = IntersectLocal(ray_local, hit_local, N_local, material);
	hit = m_LocalToGlobal * hit_local;
	N = (m_LocalToGlobal.rotation * N_local).normalized();
	return result;
#else
	return IntersectLocal(ray, hit, N, material);
#endif
}


void SceneRectangles::SetPositioning(const Affine3& positioning)
{
	m_LocalToGlobal = positioning;
	m_GlobalToLocal = positioning.inverse();
}


SceneRectangles::VectorGraphicsPlane SceneRectangles::LoadFromFile(const std::string& filepath)
{
	std::fstream filestream;

	VectorGraphicsPlane result;

	try
	{
		filestream.open( filepath, std::ofstream::in );

		if ( !filestream.is_open() )
			throw;

		if ( filestream.eof() )
			throw;

		std::vector<MinMax2Real> rectangles;

		while ( !filestream.eof() )
		{
			MinMax2Real newitem;
			filestream >> newitem.min.x >> newitem.min.y >> newitem.max.x >> newitem.max.y;
			rectangles.push_back(newitem);
		}
		filestream.close();

		result.rectangles = rectangles;
		for ( auto rect = rectangles.begin(); rect != rectangles.end(); ++rect )
		{
			result.rectangles_bounds = result.rectangles_bounds.add(*rect);
		}
	}
	catch ( ... )
	{
		return VectorGraphicsPlane();
		filestream.close();
	}

	return result;
}


bool SceneRectangles::IntersectLocal(const Ray3& ray, Vec3& hit, Vec3& N, Material& material) const
{
	Real t0 = RealMax;

	for ( auto plane = planes.begin(); plane != planes.end(); ++plane )
	{
		Real cur_t;
		Material cur_mat;
		if ( plane->ray_intersect(ray,cur_t,cur_mat) && cur_t < t0 )
		{
			t0 = cur_t;
			material = cur_mat;
			hit = ray.origin + t0*ray.direction;
			N = Vec3(0,0,1);
		}
	}

	return t0 < RealMax;
}


bool SceneRectangles::VectorGraphicsPlane::ray_intersect( const Ray3& ray, Real& t0, Material& material ) const
{
	if ( abs(ray.direction.z) < epsilon )
		return false;

	const Real t = (z - ray.origin.z) / ray.direction.z;
	if ( t < 0 )
		return false;

	const Vec2 xyWorld = ray.origin.xy + t*ray.direction.xy;
	if ( !xydim.isInRange(xyWorld) )
		return false;

	t0 = t;

	const Vec2 xyLocal = rectangles_bounds.min + (xyWorld-xydim.min)/xydim.range()*rectangles_bounds.range();

	for ( auto rect = rectangles.begin(); rect != rectangles.end(); ++rect )
	{
		if ( rect->isInRange(xyLocal) )
		{
			material = materialFilled;
			return true;
		}
	}

	material = materialEmpty;
	return true;
}

} // namespace LFOptics