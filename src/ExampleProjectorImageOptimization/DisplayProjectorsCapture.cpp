#include "DisplayProjectorsCapture.h"

DisplayProjectorsCapture::DisplayProjectorsCapture( const DisplayProjectorAligned* model )
	:DisplayModel(model)
{
}


Real DisplayProjectorsCapture::GenerateRay( const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir ) const
{
	if ( DisplayModel == nullptr )
		return 0;

	const Real lambdaX = -1.0 + 2.0 * raster.x / (Real)DisplayModel->ProjectorResolution[0];
	const Real lambdaY =  1.0 - 2.0 * raster.y / (Real)DisplayModel->ProjectorResolution[1];

	// Screen position.
	const Real x0 = lambdaX * DisplayModel->HalfPhysSize[0];
	const Real y0 = lambdaY * DisplayModel->HalfPhysSize[1];
	const Real z0 = DisplayModel->ViewerDistance;

	// Direction from projector to the ray-screen intersection.
	const Real dx = x0 - ProjectorPosition[0];
	const Real dy = y0 - ProjectorPosition[1];
	const Real dz = z0 - ProjectorPosition[2];

	// Viewer position on the observer line.
	Real X = 0;
	switch ( DisplayModel->DiffuserType )
	{
	case DisplayProjectorAligned::Diffuser::Linear: {
		// X is determined as the intersection between 2D lines (x,0) and (x0,z0)+t*(dx,dz).
		X = x0 - z0*dx/dz;
		} break;
	case DisplayProjectorAligned::Diffuser::Conical: {
		// (X,0,0) must be the intersection between cone (x-x0)^2=rho^2*((y-y0)^2+(z-z0)^2) with observer line (x,0,0).
		// In other words, we solve equation (X-x0)^2=rho^2 * (y0^2 + z0^2).
		// Variable rho is determined as rho = dx / sqrt(dy^2 + dz^2).
		const Real rho = dx / std::sqrt(dy*dy+dz*dz);
		X = x0 + rho*std::sqrt(y0*y0+z0*z0);
		} break;
	default:
		break;
	}

	ori.x = X;
	ori.y = 0;
	ori.z = 0;
	dir.x = x0 - X;
	dir.y = y0;
	dir.z = z0;

	return 1;
}
