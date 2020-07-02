#ifndef RAYGENPINHOLE_H
#define RAYGENPINHOLE_H

#include "LFRayTracer.h"


// Simple implementation of perspective pinhole camera.
// Image plane is at z=ImagePlaneDepth.
// Ray origin is (X,Y,0).
// Ray direction is from origin towards point on the image plane.
class RayGenPinhole : public lfrt::RayGenerator
{
public:
	using Real = lfrt::Real;
	using Int = lfrt::Int;
	using VEC2 = lfrt::VEC2;
	using VEC3 = lfrt::VEC3;

	RayGenPinhole() = default;

	RayGenPinhole( const Int& width, const Int& height );

	RayGenPinhole(
		const Int& width, const Int& height,
		const Real& imagePlaneHalfSizeX,
		const Real& imagePlaneHalfSizeY,
		const Real& imagePlaneDepth );

	RayGenPinhole(
		const Int& width, const Int& height,
		const Real& minX, const Real& minY,
		const Real& maxX, const Real& maxY,
		const Real& imagePlaneDepth );

	RayGenPinhole(
		const Int& width, const Int& height,
		const Real& minX, const Real& minY,
		const Real& maxX, const Real& maxY,
		const Real& imagePlaneDepth,
		const Real& originX, const Real& originY );

	virtual ~RayGenPinhole() = default;

	virtual Real GenerateRay(
		const VEC2& raster, const VEC2& secondary,
		VEC3& ori, VEC3& dir ) const override;

	Real ImagePlaneDepth = 1.0;
	Real MinX = -1.0;
	Real MinY = -1.0;
	Real MaxX = 1.0;
	Real MaxY = 1.0;
	Int Width = 512;
	Int Height = 512;
	Real OriginX = 0;
	Real OriginY = 0;
};



#endif // RAYGENPINHOLE_H