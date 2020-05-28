#pragma once


#include "BaseTypes.h"
#include "BaseInterfaces.h"



namespace LFOptics
{


class ColorPanelInIm : public IRayGenerator, public ILFSystem3
{
public:
	ColorPanelInIm();

	MinMax2Real& PanelSize() { return m_PanelSize; }
	Real& ImagePlaneZ() { return m_ImagePlaneZ; }
	Real& InvLensletFocusDistance() { return m_InvLensletFocusDistance; }

	const MinMax2Real& PanelSize() const { return m_PanelSize; }
	const Real& ImagePlaneZ() const { return m_ImagePlaneZ; }
	const Real& InvLensletFocusDistance() const { return m_InvLensletFocusDistance; }

	// From patch index to surface coord.
	const Affine2& PatchIndexToSurface() const { return m_PatchIndexToSurface; }
	const Affine2& SurfaceToPatchIndex() const { return m_SurfaceToPatchIndex; }

	// From lenslet index to surface coord.
	const Affine2& LensletIndexToSurface() const { return m_LensletIndexToSurface; }
	const Affine2& SurfaceToLensletIndex() const { return m_SurfaceToLensletIndex; }

	void SetPatchIndexToSurface( const Affine2& patchIndexToSurface ); // Supply positioning of patch by index.
	void SetLensletIndexToSurface( const Affine2& lensletIndexToSurface ); // Supply positioning of lenslet by index.

	// The following two functions should be called only after PanelSize is set or updated.
	void SetPatchToSurfaceByPatchSize( const Vec2& patchSizeInSurface, const Vec2& patchCenterInSurface );
	void SetLensletToSurfaceByLensletSize( const Vec2& lensletSizeInSurface, const Vec2& lensletCenterInSurface );

	Vec2i LensletIndexByLensletSurface( const Vec2& lensletSurfaceCoord ) const;
	Vec2i PatchIndByImageSurface( const Vec2& imageSurfaceCoord ) const;

	Vec2i LensletIndex( const Vec4& lfcoord ) const;
	Vec2i PatchIndex( const Vec4& lfcoord ) const;

	//Ray3 CreateRay( const Vec2& imageSurfaceCoord, const Vec2& lensletLocalCoord = Vec2(0,0) ) const;

	// Inherited via IRayGenerator
	//virtual Ray3 GenerateRay( const Vec2& surfaceCoord ) const override { return CreateRay( surfaceCoord ); }
	virtual Ray3 GenerateRay( const Vec2& surfaceCoord ) const override { return LFToRay3(Vec4(surfaceCoord,0,0)); }

	// Inherited via ILFSystem3
	virtual Vec4 Ray3ToLF( const Ray3& ray ) const override;
	virtual Ray3 LFToRay3( const Vec4& coord ) const override;

	//// Returns false if ray does not hit the surface.
	//bool IntersectRay( const Ray3& ray, Vec2& imageSurfaceCoord ) const;
	//bool IntersectRay( const Ray3& ray, Vec2& imageSurfaceCoord, Vec2& lensletLocalCoord, Vec2i& lensletInd ) const;

	Vec2 SurfaceCoord( const Vec3& point, const Vec2i& lensletInd ) const;

private:
	inline Vec2 RefractRayDir2Local( const Vec2& raydir2, const Vec2& lensletLocalCoord, const Real& zDirSign ) const;

private:
	MinMax2Real m_PanelSize;
	Real m_ImagePlaneZ = Real(-1); // Z-position of the image plane. In local coords.
	Real m_InvLensletFocusDistance = Real(1); // Inverse focal distance of objective lens.

	Affine2 m_PatchIndexToSurface; // From patch index to surface coord.
	Affine2 m_SurfaceToPatchIndex;
	Affine2 m_LensletIndexToSurface; // From lenslet index to surface coord.
	Affine2 m_SurfaceToLensletIndex;
};


} // namespace LFOptics