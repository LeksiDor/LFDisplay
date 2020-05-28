#pragma once


#include "BaseTypes.h"
#include "ImageLayout.h"
#include "ColorPanelInIm.h"

#include "BaseInterfaces.h"


#include <string>
#include <vector>


using namespace LFOptics;


namespace LFOptics { class Image2D; }


// Near-eye Integral Imaging display.
class LFDisplay05
{
public:
	class DisplaySimulation;
	class RayGeneration;

	enum class Sampling
	{
		LensletCenter = 0,
		PupilCenter = 1,
		LensletAverage = 2,
	};

public:
	LFDisplay05();
	~LFDisplay05();

	PixelInterpolation& GetPixelInterpolation() { return m_PixelInterpolation; }
	SubpixelType& GetSubpixelType() { return m_SubpixelType; }
	bool& IsUseBarriers() { return m_IsUseBarriers; }
	ColorPanelInIm& ColorPanel() { return m_ColorPanel; }
	Affine3& Positioning() { return m_Positioning; }

	const PixelInterpolation& GetPixelInterpolation() const { return m_PixelInterpolation; }
	const SubpixelType& GetSubpixelType() const { return m_SubpixelType; }
	const bool& IsUseBarriers() const { return m_IsUseBarriers; }
	const ColorPanelInIm& ColorPanel() const { return m_ColorPanel; }
	const Affine3& Positioning() const { return m_Positioning; }

	const ImageLayout& GetImageLayout() const { return m_ImageLayout; }

	DisplaySimulation CreateDisplaySimulation( const Image2D& displayimage ) const;
	RayGeneration CreateRayGeneration( const Vec3& eyePos, const Sampling& sampling ) const;

	// Updates image-to-surface transformation.
	bool UpdateImageLayout( const Vec2i& imageRes );

public:
	class DisplaySimulation : public IRayColorizer
	{
	public:
		const LFDisplay05& lfdisplay;
		const Image2D& displayimage;
		const Affine3 worldToDisplay;
		DisplaySimulation( const LFDisplay05& lfdisplay, const Image2D& displayimage )
			:lfdisplay(lfdisplay)
			,displayimage(displayimage)
			,worldToDisplay(lfdisplay.m_Positioning.inverse()) {}
		virtual Vec4 ProcessRay( const Ray3& ray ) const override;
	};

	class RayGeneration : public IRayGeneratorOnImageMulti
	{
	public:
		const LFDisplay05& lfdisplay;
		const Vec3 eyePos;
		const Affine3 displayToWorld;
		const Sampling sampling;
		Int numSamplesOnLensletInRow = 5;
		RayGeneration( const LFDisplay05& lfdisplay, const Vec3& eyePos, const Sampling& sampling )
			:lfdisplay(lfdisplay)
			,eyePos(eyePos)
			,sampling(sampling)
			,displayToWorld(lfdisplay.m_Positioning) {}
		virtual Int NumSamples() const override;
		virtual void GenerateRays(std::vector<Vec4>& colorWeights, std::vector<Ray3>& rays, const Vec2i& imageCoord) const override;
	};

private:
	PixelInterpolation m_PixelInterpolation = PixelInterpolation::Trapezoidal;
	SubpixelType m_SubpixelType = SubpixelType::RegularHor;
	bool m_IsUseBarriers = true;

	ImageLayout m_ImageLayout;
	ColorPanelInIm m_ColorPanel;

	Affine3 m_Positioning;
};

