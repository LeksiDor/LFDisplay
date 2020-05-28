#include "LFDisplay05.h"

#include "Image2D.h"

#include <fstream>
#include <iomanip>



LFDisplay05::LFDisplay05()
{
}


LFDisplay05::~LFDisplay05()
{
}


LFDisplay05::DisplaySimulation LFDisplay05::CreateDisplaySimulation( const Image2D& displayimage ) const
{
	return DisplaySimulation( *this, displayimage );
}


LFDisplay05::RayGeneration LFDisplay05::CreateRayGeneration( const Vec3& eyePos, const Sampling& sampling ) const
{
	return RayGeneration( *this, eyePos, sampling );
}


bool LFDisplay05::UpdateImageLayout( const Vec2i& imageRes )
{
	if ( imageRes.x <= 0 || imageRes.y <= 0 )
		return false;
	if ( imageRes.x == IntNaN || imageRes.y == IntNaN )
		return false;
	m_ImageLayout.Resize(1);
	m_ImageLayout.ImageLayer(0) = 0;
	m_ImageLayout.ImageMinMax(0) = MinMax2Int(0,0,imageRes.x,imageRes.y);
	const Vec2 imageResReal = Vec2(imageRes.x,imageRes.y);
	const MinMax2Real screenMinMax = m_ColorPanel.PanelSize();
	const Vec2 screenSize = screenMinMax.range();
	const Vec2 scale = screenSize / imageResReal;
	const Affine2 transformInvertY = Affine2( Mat22(1,0,0,-1), Vec2(0,imageRes.y) );
	const Affine2 imageToSurface = Affine2( Mat22(scale.x,0,0,scale.y), screenMinMax.min ) * transformInvertY;
	m_ImageLayout.ImageToSurface(0) = imageToSurface;
	m_ImageLayout.SurfaceToImage(0) = imageToSurface.inverse();
	return true;
}



Vec4 LFDisplay05::DisplaySimulation::ProcessRay( const Ray3& rayGlobal ) const
{
	const ColorPanelInIm& colorpanel = lfdisplay.ColorPanel();
	const Ray3 rayLocal = worldToDisplay * rayGlobal;

	const Vec4 lfcoord = colorpanel.Ray3ToLF(rayLocal);
	const Vec2& imageSurfaceCoord = lfcoord.xy;

	const Affine2& imageSurfaceToImage = lfdisplay.GetImageLayout().SurfaceToImage(0);
	const Vec2 imageCoord = imageSurfaceToImage * imageSurfaceCoord;
	if ( lfdisplay.IsUseBarriers() )
	{
		const Vec2i lensletInd = colorpanel.LensletIndex( lfcoord );
		const Vec2i patchInd = colorpanel.PatchIndex( lfcoord );
		if ( lensletInd.x != patchInd.x || lensletInd.y != patchInd.y )
			return Vec4(0,0,0,0);
	}
	const Vec4 color = displayimage.at( imageCoord.x, imageCoord.y, lfdisplay.GetPixelInterpolation() );
#if 0
	return color;
#else
	if (imageCoord.x >= 0 && imageCoord.y >= 0 && imageCoord.x <= displayimage.Width() && imageCoord.y <= displayimage.Height())
		return color;
	else
		return Vec4(0,0,0,1);
#endif
}


Int LFDisplay05::RayGeneration::NumSamples() const
{
	switch (sampling)
	{
	case Sampling::LensletCenter: {
		return 1;
	}	break;
	case Sampling::PupilCenter: {
		return 1;
	}	break;
	case Sampling::LensletAverage: {
		return numSamplesOnLensletInRow * numSamplesOnLensletInRow;
	}	break;
	default:
		return 0;
		break;
	}
}

void LFDisplay05::RayGeneration::GenerateRays(std::vector<Vec4>& colorWeights, std::vector<Ray3>& rays, const Vec2i& imageCoord) const
{
	const Int numSamples = NumSamples();
	colorWeights.resize(numSamples);
	rays.resize(numSamples);

	const ColorPanelInIm& panel = lfdisplay.ColorPanel();

	switch (sampling)
	{
	case Sampling::LensletCenter: {

		const Affine2& imageToImageSurface = lfdisplay.GetImageLayout().ImageToSurface(0);
		const Vec2 imagesurfaceCoord = imageToImageSurface * imageCoord.toType<Real>();
		const Ray3 rayFromPanel = panel.LFToRay3(Vec4(imagesurfaceCoord,0,0));
		const Vec2 raydir2 = rayFromPanel.direction.xy / rayFromPanel.direction.z;
		const Vec2 rayori2 = rayFromPanel.origin.xy + eyePos.z * raydir2;
		const Ray3 rayLocal = Ray3( Vec3(rayori2,eyePos.z), -Vec3(raydir2,1) );
		const Ray3 rayGlobal = displayToWorld * rayLocal;

		const Int sampleInd = 0;
		rays[sampleInd] = rayGlobal;
		colorWeights[sampleInd] = Vec4(1,1,1,1);

	}	break;

	case Sampling::PupilCenter: {

		const Affine2& imageToImageSurface = lfdisplay.GetImageLayout().ImageToSurface(0);
		const Vec2 imagesurfaceCoord = imageToImageSurface * imageCoord.toType<Real>();
		const Vec2i lensletInd = panel.PatchIndByImageSurface(imagesurfaceCoord);
		const Vec2 lensletCenter = panel.LensletIndexToSurface() * lensletInd.toType<Real>();
		const Real opticalpower = panel.InvLensletFocusDistance();
		const Real dist_lcd = abs(panel.ImagePlaneZ());
		const Real dist_eye = abs(eyePos.z);

		const Vec2 nominator = imagesurfaceCoord - eyePos.xy + opticalpower*dist_lcd*(eyePos.xy - lensletCenter);
		const Real denominator = dist_eye + dist_lcd - dist_eye*dist_lcd*opticalpower;
		const Vec2 raydir2 = - nominator / denominator;

		const Ray3 rayLocal = Ray3( eyePos, -Vec3(raydir2,1) );
		const Ray3 rayGlobal = displayToWorld * rayLocal;

		const Int sampleInd = 0;
		rays[sampleInd] = rayGlobal;
		colorWeights[sampleInd] = Vec4(1,1,1,1);

	}	break;

	case Sampling::LensletAverage: {

		const Affine2& imageToImageSurface = lfdisplay.GetImageLayout().ImageToSurface(0);
		const Vec2 imagesurfaceCoord = imageToImageSurface * imageCoord.toType<Real>();
		const Vec2i lensletInd = panel.PatchIndByImageSurface(imagesurfaceCoord);
		const Vec2 lensletCenter = panel.LensletIndexToSurface() * lensletInd.toType<Real>();
		const Real opticalpower = panel.InvLensletFocusDistance();
		const Real dist_lcd = abs(panel.ImagePlaneZ());
		const Real dist_eye = abs(eyePos.z);

		const Vec2 lensletSize = Vec2( panel.LensletIndexToSurface().rotation(0,0), panel.LensletIndexToSurface().rotation(1,1) );

		for ( int xInd = 0; xInd < numSamplesOnLensletInRow; ++xInd )
		{
			for ( int yInd = 0; yInd < numSamplesOnLensletInRow; ++yInd )
			{
				const Vec2 lambda = -Vec2(0.5,0.5) + (Vec2(xInd,yInd) + Vec2(0.5,0.5)) / Real(numSamplesOnLensletInRow);
				const Vec2 lensletCoord = lensletCenter + lambda*lensletSize;
				const Vec2 raydir2 = (imagesurfaceCoord - lensletCoord)/dist_lcd + (lensletCoord-lensletCenter)*opticalpower;
				const Vec2 rayori2 = lensletCoord - dist_eye*raydir2;

				const Ray3 rayLocal = Ray3( Vec3(rayori2,eyePos.z), Vec3(raydir2,-1) );
				const Ray3 rayGlobal = displayToWorld * rayLocal;

				const Int sampleInd = xInd + yInd*numSamplesOnLensletInRow;
				rays[sampleInd] = rayGlobal;
				colorWeights[sampleInd] = Vec4(1,1,1,1);
			}
		}

	}	break;
	default:
		break;
	}
}
