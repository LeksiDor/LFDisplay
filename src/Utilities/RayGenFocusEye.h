#ifndef UTILITIES_RAYGENFOCUSEYE_H
#define UTILITIES_RAYGENFOCUSEYE_H

#include "LFRayTracer.h"


// Simple implementation of two-plane eye model able to focus.
// Retina plane is at z=RetinaPlaneZ.
// Aperture center is (0,0,0).
// Rays originate from aperture plane at z=0.
// Rays corresponding to the same retina point intersect at z=InFocusPlaneZ.
// Ray direction is from origin towards point on the image plane.
class RayGenFocusEye : public lfrt::RayGenerator
{
public:
	using Real = lfrt::Real;
	using Int = lfrt::Int;
	using VEC2 = lfrt::VEC2;
	using VEC3 = lfrt::VEC3;

	RayGenFocusEye() = default;

	virtual ~RayGenFocusEye() = default;

	virtual Real GenerateRay(
		const VEC2& raster, const VEC2& secondary,
		VEC3& ori, VEC3& dir ) const override;

	Real RetinaPlaneZ = -1.0;
	Real InFocusPlaneZ = 1.0;
	VEC2 RetinaStart = VEC2({ -1.0, -1.0 });
	VEC2 RetinaEnd   = VEC2({  1.0,  1.0 });
	Real ApertureRadius = 1.0;
	Int Width = 512;
	Int Height = 512;
};

#endif // UTILITIES_RAYGENFOCUSEYE_H