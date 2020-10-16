#include "RayGenFocusEye.h"

using namespace lfrt;


Real RayGenFocusEye::GenerateRay( const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir ) const
{
	if ( raster.x < 0 || raster.y < 0 || raster.x > Width || raster.y > Height )
		return 0.0;

	const VEC2 lambda({ raster.x / Real(Width), 1.0 - raster.y / Real(Height) });
	const VEC2 retinaPos({
		RetinaStart.x + lambda.x * (RetinaEnd.x - RetinaStart.x),
		RetinaStart.y + lambda.y * (RetinaEnd.y - RetinaStart.y) });
	const VEC2 aperturePos({
		(-1.0 + 2.0*secondary.x)*ApertureRadius,
		(-1.0 + 2.0*secondary.y)*ApertureRadius });

	// Find intersection between ray-through-aperture-center and in-focus-plane.
	const Real coef = InFocusPlaneZ / RetinaPlaneZ;
	const VEC2 targetPos({ retinaPos.x*coef, retinaPos.y*coef });

	// Trace ray from aperture position to point-in-focus.
	ori.x = aperturePos.x;
	ori.y = aperturePos.y;
	ori.z = 0;
	dir.x = targetPos.x - aperturePos.x;
	dir.y = targetPos.y - aperturePos.y;
	dir.z = InFocusPlaneZ;

	return 1.0;
}