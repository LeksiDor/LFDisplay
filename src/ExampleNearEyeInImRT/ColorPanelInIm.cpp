#include "ColorPanelInIm.h"

#include <cmath>


namespace LFOptics
{


ColorPanelInIm::ColorPanelInIm()
{
}


void ColorPanelInIm::SetPatchIndexToSurface( const Affine2& patchIndexToSurface )
{
	m_PatchIndexToSurface = patchIndexToSurface;
	m_SurfaceToPatchIndex = patchIndexToSurface.inverse();
}


void ColorPanelInIm::SetLensletIndexToSurface( const Affine2& lensletIndexToSurface )
{
	m_LensletIndexToSurface = lensletIndexToSurface;
	m_SurfaceToLensletIndex = lensletIndexToSurface.inverse();
}


void ColorPanelInIm::SetPatchToSurfaceByPatchSize( const Vec2& patchSizeInSurface, const Vec2& patchCenterInSurface )
{
	SetPatchIndexToSurface(Affine2( Mat22::diagonalize(patchSizeInSurface), patchCenterInSurface ));
}


void ColorPanelInIm::SetLensletToSurfaceByLensletSize( const Vec2& lensletSizeInSurface, const Vec2& lensletCenterInSurface )
{
	SetLensletIndexToSurface(Affine2( Mat22::diagonalize(lensletSizeInSurface), lensletCenterInSurface ));
}


Vec2i ColorPanelInIm::LensletIndex( const Vec4& lfcoord ) const
{
	const Vec2i patchInd = PatchIndByImageSurface( lfcoord.xy );
	const Vec2 lensletCenter = m_LensletIndexToSurface * Vec2(patchInd.x,patchInd.y);
	const Vec2 lensletSurfaceCoord = lensletCenter + lfcoord.zw;
	const Vec2i lensletInd = LensletIndexByLensletSurface( lensletSurfaceCoord );
	return lensletInd;
}


Vec2i ColorPanelInIm::PatchIndex( const Vec4& lfcoord ) const
{
	return PatchIndByImageSurface( lfcoord.xy );
}


Vec2i ColorPanelInIm::LensletIndexByLensletSurface( const Vec2& lensletSurfaceCoord ) const
{
	const Vec2 lensletCoord = m_SurfaceToLensletIndex * lensletSurfaceCoord;
	return Vec2i( std::lround(lensletCoord.x), std::lround(lensletCoord.y) );
}


Vec2i ColorPanelInIm::PatchIndByImageSurface( const Vec2& imageSurfaceCoord ) const
{
	const Vec2 patchCoord = m_SurfaceToPatchIndex * imageSurfaceCoord;
	return Vec2i( std::lround(patchCoord.x), std::lround(patchCoord.y) );
}


//Ray3 ColorPanelInIm::CreateRay( const Vec2& imageSurfaceCoord, const Vec2& lensletLocalCoord ) const
//{
//	const Vec2i patchInd = PatchIndByImageSurface( imageSurfaceCoord );
//	const Vec2i lensletInd = patchInd;
//	const Vec2 lensletCenterInSurface = m_LensletIndexToSurface * Vec2(lensletInd.x,lensletInd.y);
//	const Vec2 lensletSurfaceCoord = lensletCenterInSurface + lensletLocalCoord;
//	const Vec3 rayori = Vec3( lensletSurfaceCoord, 0 );
//	const Real zDirSign = (m_ImagePlaneZ < 0) ? 1.0 : -1.0;
//	const Vec2 raydir2 = (lensletSurfaceCoord - imageSurfaceCoord) / (-m_ImagePlaneZ);
//	const Vec2 refractedDir2 = RefractRayDir2Local( raydir2, lensletLocalCoord, zDirSign );
//	const Vec3 refractedDir = zDirSign*Vec3(refractedDir2,1);
//	return Ray3( rayori, refractedDir );
//}


//bool ColorPanelInIm::IntersectRay( const Ray3& ray, Vec2& imageSurfaceCoord ) const
//{
//	Vec2 lensletLocalCoord;
//	Vec2i lensletInd;
//	return IntersectRay( ray, imageSurfaceCoord, lensletLocalCoord, lensletInd );
//}
//
//
//bool ColorPanelInIm::IntersectRay( const Ray3& ray, Vec2& imageSurfaceCoord, Vec2& lensletLocalCoord, Vec2i& lensletInd ) const
//{
//	const Vec3& rayori = ray.origin;
//	const Vec3& raydir = ray.direction;
//	const Vec2 raydir2 = raydir.xy / raydir.z;
//	const Vec2 lensletSurfaceCoord = rayori.xy - rayori.z * raydir2;
//	const bool result = m_PanelSize.isInRange( lensletSurfaceCoord );
//	lensletInd = LensletIndexByLensletSurface( lensletSurfaceCoord );
//	const Vec2 lensletCenterInSurface = m_LensletIndexToSurface * Vec2(lensletInd.x,lensletInd.y);
//	lensletLocalCoord = lensletSurfaceCoord - lensletCenterInSurface;
//	const Real zDirSign = (raydir.z > 0) ? 1.0 : -1.0;
//	const Vec2 refractedDir2 = RefractRayDir2Local( raydir2, lensletLocalCoord, zDirSign );
//	imageSurfaceCoord = lensletSurfaceCoord + refractedDir2 * m_ImagePlaneZ;
//	return result;
//}


Vec4 ColorPanelInIm::Ray3ToLF( const Ray3& ray ) const
{
	const Vec3& rayori = ray.origin;
	const Vec3& raydir = ray.direction;
	const Vec2 raydir2 = raydir.xy / raydir.z;
	const Vec2 lensletSurfaceCoord = rayori.xy - rayori.z * raydir2;
	const bool result = m_PanelSize.isInRange( lensletSurfaceCoord );
	const Vec2i lensletInd = LensletIndexByLensletSurface( lensletSurfaceCoord );
	const Vec2 lensletCenterInSurface = m_LensletIndexToSurface * Vec2(lensletInd.x,lensletInd.y);
	const Vec2 lensletLocalCoord = lensletSurfaceCoord - lensletCenterInSurface;
	const Real zDirSign = (raydir.z > 0) ? 1.0 : -1.0;
	const Vec2 refractedDir2 = RefractRayDir2Local( raydir2, lensletLocalCoord, zDirSign );
	const Vec2 imageSurfaceCoord = lensletSurfaceCoord + refractedDir2 * m_ImagePlaneZ;
	return Vec4( imageSurfaceCoord, lensletLocalCoord );
}

Ray3 ColorPanelInIm::LFToRay3( const Vec4& coord ) const
{
	const Vec2& imageSurfaceCoord = coord.xy;
	const Vec2& lensletLocalCoord = coord.zw;
	const Vec2i patchInd = PatchIndByImageSurface( imageSurfaceCoord );
	const Vec2i lensletInd = patchInd;
	const Vec2 lensletCenterInSurface = m_LensletIndexToSurface * Vec2(lensletInd.x,lensletInd.y);
	const Vec2 lensletSurfaceCoord = lensletCenterInSurface + lensletLocalCoord;
	const Vec3 rayori = Vec3( lensletSurfaceCoord, 0 );
	const Real zDirSign = (m_ImagePlaneZ < 0) ? 1.0 : -1.0;
	const Vec2 raydir2 = (lensletSurfaceCoord - imageSurfaceCoord) / (-m_ImagePlaneZ);
	const Vec2 refractedDir2 = RefractRayDir2Local( raydir2, lensletLocalCoord, zDirSign );
	const Vec3 refractedDir = zDirSign*Vec3(refractedDir2,1);
	return Ray3( rayori, refractedDir );
}


Vec2 ColorPanelInIm::SurfaceCoord( const Vec3& point, const Vec2i& lensletInd ) const
{
	const Vec2 lensletCenterInSurface = m_LensletIndexToSurface * Vec2(lensletInd.x,lensletInd.y);
	const Vec2 raydir2 = (point.xy - lensletCenterInSurface) / point.z;
	const Vec2 imageSurfaceCoord = lensletCenterInSurface + raydir2 * m_ImagePlaneZ;
	return imageSurfaceCoord;
}


Vec2 ColorPanelInIm::RefractRayDir2Local( const Vec2& raydir2, const Vec2& lensletLocalCoord, const Real& zDirSign ) const
{
	return raydir2 - (zDirSign*m_InvLensletFocusDistance)*lensletLocalCoord;
}


} // namespace LFOptics