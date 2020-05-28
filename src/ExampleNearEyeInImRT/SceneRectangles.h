#pragma once

#include "RendererBaseTypes.h"

#include <string>

namespace LFOptics
{

class SceneRectangles : public Scene
{
public:
	struct VectorGraphicsPlane
	{
		// External parameters.
		MinMax2Real xydim;
		Real z = 0;
		// Internal parameters.
		std::vector<MinMax2Real> rectangles;
		MinMax2Real rectangles_bounds;
		// Other parameters.
		Material materialEmpty = Material(1.0, Vec4(1,0,0,0), Vec3(1,1,1), 1, false, false, true);
		Material materialFilled = Material(1.0, Vec4(1,0,0,0), Vec3(0,0,0), 1, false, false, true);
		// Functions.
		bool ray_intersect( const Ray3& ray, Real& t0, Material& material ) const;
	};

public:
	SceneRectangles( const Affine3& positioning = Affine3::identity() );

	// Inherited via Scene
	virtual bool Intersect(const Ray3& ray, Vec3& hit, Vec3& N, Material& material) const override;

	void SetPositioning(const Affine3& positioning);
	static VectorGraphicsPlane LoadFromFile(const std::string& filepath);

public:
	std::vector<VectorGraphicsPlane> planes;

private:
	bool IntersectLocal(const Ray3& ray, Vec3& hit, Vec3& N, Material& material) const;

private:
	Affine3 m_LocalToGlobal = Affine3::identity();
	Affine3 m_GlobalToLocal = Affine3::identity();
};

} // namespace LFOptics