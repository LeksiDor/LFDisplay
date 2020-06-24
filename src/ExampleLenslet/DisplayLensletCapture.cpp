#include "DisplayLensletCapture.h"



DisplayLensletCapture::DisplayLensletCapture( const DisplayLenslet* model )
	:DisplayModel(model)
{
}


Real DisplayLensletCapture::GenerateRay( const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir ) const
{
	if ( DisplayModel == nullptr )
		return 0;

	const Vec2i& resolution = DisplayModel->ResolutionLCD;

	if ( raster.x < 0 || raster.x > resolution[0] ||
		 raster.y < 0 || raster.y > resolution[1] )
		return 0;

	const Vec2 eiLambda = Vec2(
		-0.5 + raster.x / Real(resolution[0]),
		 0.5 - raster.y / Real(resolution[1]) );
	const Vec2& lcdSize = DisplayModel->SizeLCD;
	const Vec2 eiPos = Vec2( eiLambda[0]*lcdSize[0], eiLambda[1]*lcdSize[1] );
	const Vec2& eiShiftInv = DisplayModel->EIShiftInv();
	const Mat22& eiOrientationInv = DisplayModel->EIOrientationInv();
	const Vec2 eiIndReal = eiShiftInv + eiOrientationInv * eiPos;
	const Vec2i eiInd = Vec2i( std::round(eiIndReal[0]), std::round(eiIndReal[1]) );

	const Vec2i lensletInd = eiInd;
	const Vec2 lensletIndReal = lensletInd;
	const Vec2& lensletShift = DisplayModel->LensletShift();
	const Mat22& lensletOrientation = DisplayModel->LensletOrientation();
	const Vec2 lensletCenter = lensletShift + lensletOrientation * lensletIndReal;
	const bool isLensletVertical = DisplayModel->IsLensletVertical;

	//Vec2 rayDirTan; // (dir.x,dir.y)/dir.z
	const Real f = DisplayModel->LensletToOrigin;
	const Real d = DisplayModel->LensletToLCD;
	const Vec3 eiPos3d = Vec3( eiPos[0], eiPos[1], f+d );

	Vec2 rayDirTan;
	if ( isLensletVertical )
	{
		// Ray starts from observer line and goes through lenslet central axis.
		const Vec2 vertLensletDir = Vec2( lensletOrientation(0,1), lensletOrientation(1,1) );
		const Vec2 diff = eiPos - lensletCenter;
		const Real lambda = ( diff[1] - d/(f+d)*eiPos[1] ) / vertLensletDir[1];
		const Vec2 lensletPos = lensletCenter + lambda * vertLensletDir;
		rayDirTan = ( eiPos - lensletPos ) / d;
	}
	else
	{
		// Ray starts from observer plane and goes through lenslet center.
		rayDirTan = ( eiPos - lensletCenter ) / d;
	}

	const Vec2 viewerPos = lensletCenter - f * rayDirTan;
	ori.x = viewerPos[0];
	ori.y = viewerPos[1];
	ori.z = 0;
	dir.x = rayDirTan[0];
	dir.y = rayDirTan[1];
	dir.z = 1;
	return 1;
}
