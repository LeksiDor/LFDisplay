#include "DisplayLensletCapture.h"



DisplayLensletCapture::DisplayLensletCapture( const DisplayLenslet* model, const Sampling& sampling )
	:DisplayModel(model)
	,SamplingType(sampling)
{
}


Real DisplayLensletCapture::GenerateRay( const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir ) const
{
	if ( DisplayModel == nullptr )
		return 0;

	const Vec3 eyePos = Vec3(0,0,0);

	const Vec2i& resolution = DisplayModel->ResolutionLCD;

	if ( raster.x < 0 || raster.x > resolution[0] ||
		 raster.y < 0 || raster.y > resolution[1] )
		return 0;

	const Vec2 eiLambda = Vec2(
		-0.5 + raster.x / Real(resolution[0]),
		 0.5 - raster.y / Real(resolution[1]) );
	const Vec2& lcdSize = DisplayModel->SizeLCD;
	const Vec2 eiPos2D = Vec2( eiLambda[0]*lcdSize[0], eiLambda[1]*lcdSize[1] );
	const Vec2& eiShiftInv = DisplayModel->EIShiftInv();
	const Mat22& eiOrientationInv = DisplayModel->EIOrientationInv();
	const Vec2 eiIndReal = eiShiftInv + eiOrientationInv * eiPos2D;
	const Vec2i eiInd = Vec2i( std::round(eiIndReal[0]), std::round(eiIndReal[1]) );

	const Vec2i lensletInd = eiInd;
	const Vec2 lensletIndReal = lensletInd;
	const Vec2& lensletShift = DisplayModel->LensletShift();
	const Mat22& lensletOrientation = DisplayModel->LensletOrientation();
	const Vec2 lensletCenter2D = lensletShift + lensletOrientation * lensletIndReal;
	//const bool isLensletVertical = DisplayModel->IsLensletVertical;

	//const Real f = DisplayModel->LensletToOrigin;
	//const Real d = DisplayModel->LensletToLCD;
	//const Vec3 eiPos3d = Vec3( eiPos2D[0], eiPos2D[1], f+d );

	//Vec2 rayDirTan;
	//if ( isLensletVertical )
	//{
	//	// Ray starts from observer line and goes through lenslet central axis.
	//	const Vec2 vertLensletDir = Vec2( lensletOrientation(0,1), lensletOrientation(1,1) );
	//	const Vec2 diff = eiPos2D - lensletCenter;
	//	const Real lambda = ( diff[1] - d/(f+d)*eiPos2D[1] ) / vertLensletDir[1];
	//	const Vec2 lensletPos = lensletCenter + lambda * vertLensletDir;
	//	rayDirTan = ( eiPos2D - lensletPos ) / d;
	//}
	//else
	//{
	//	// Ray starts from observer plane and goes through lenslet center.
	//	rayDirTan = ( eiPos2D - lensletCenter ) / d;
	//}

	//const Vec2 viewerPos = lensletCenter - f * rayDirTan;
	//ori.x = viewerPos[0];
	//ori.y = viewerPos[1];
	//ori.z = 0;
	//dir.x = rayDirTan[0];
	//dir.y = rayDirTan[1];
	//dir.z = 1;
	//return 1;

	const Real focalLength = DisplayModel->LensletFocalLength;
	const Real distLensletToLCD = DisplayModel->LensletToLCD;
	const Real distLensletToOrigin = DisplayModel->LensletToOrigin;

	Vec2 viewerPos2D( 0, 0 );
	Vec2 rayDir2D( 0, 0 );

	switch ( SamplingType )
	{
	case Sampling::LensletCenter:
	case Sampling::LensletAverage: {
		Vec2 lensletPos2D = lensletCenter2D;
		if ( SamplingType == Sampling::LensletAverage )
		{
			const Vec2 shiftArg( secondary.x - 0.5, secondary.y - 0.5 );
			lensletPos2D += lensletOrientation * shiftArg;
		}
		const Vec2 eiDir2D = (eiPos2D - lensletPos2D) / distLensletToLCD;
		rayDir2D = eiDir2D - (lensletPos2D - lensletCenter2D) / focalLength;
		viewerPos2D = lensletPos2D - rayDir2D * distLensletToOrigin;
		} break;
	case Sampling::PupilCenter: {
		// Planes: 0 -- viewer, 1 -- lenslet, 2 -- LCD.
		// x0, x1, x2 -- corresponding XY-positions relative to optical axis.
		// x0, x2 -- known, x1 -- unknown.
		// v01, v12 -- distances between planes.
		// dir01, dir12 -- tan-valued directions between planes.
		// dir01 = (x1-x0)/v01; dir12 = (x2-x1)/v12.
		// Thin lens law: dir12 = dir01 - x1/f, where f -- focal length.
		// Thus, (x2-x1)/v12 = (x1-x0)/v01 - x1/f.
		// Thus, x1 * (1/v01 + 1/v12 - 1/f) = x0/v01 + x2/v12.
		const Vec2 x0 = Vec2(eyePos[0],eyePos[1]) - lensletCenter2D;
		const Vec2 x2 = eiPos2D - lensletCenter2D;
		const Real f = focalLength;
		const Real v01 = distLensletToOrigin;
		const Real v12 = distLensletToLCD;
		const Vec2 x1 = (x0/v01 + x2/v12) / (1.0/v01 + 1.0/v12 - 1.0/f);
		const Vec2 lensletPos2D = lensletCenter2D + x1;
		viewerPos2D = Vec2( eyePos[0], eyePos[1] );
		rayDir2D = (lensletPos2D - viewerPos2D) / distLensletToOrigin;
		} break;
	default:
		break;
	}

	ori.x = viewerPos2D[0];
	ori.y = viewerPos2D[1];
	ori.z = 0;
	dir.x = rayDir2D[0];
	dir.y = rayDir2D[1];
	dir.z = 1;
	return 1;
}
