#ifndef DISPLAYLENSLETSHOW_H
#define DISPLAYLENSLETSHOW_H

#include "LFRayTracer.h"

#include <opencv2/opencv.hpp>

class DisplayLenslet;


// Class for simulating the lenslet display visual output.
class DisplayLensletShow
	: public lfrt::LFRayTracer
{
public:
	using Int = lfrt::Int;
	using Real = lfrt::Real;
	using VEC2 = lfrt::VEC2;
	using VEC3 = lfrt::VEC3;
	using Color = cv::Vec3f;

public:

	DisplayLensletShow( const DisplayLenslet* displayModel = nullptr );

	virtual ~DisplayLensletShow() = default;

	// Filepath is location of the display image.
	virtual bool LoadScene( const std::string& filepath ) override;

	// Returns simple pinhole camera.
	virtual lfrt::RayGenerator* CreateDefaultRayGenerator( const Int& width, const Int& height ) const override;

	// Returns uniform sample generator.
	virtual lfrt::SampleGenerator* CreateDefaultSampleGenerator( const Int& width, const Int& height ) const override;

	// Returns OpenCV-based sample accum.
	virtual lfrt::SampleAccumulator* CreateDefaultSampleAccumulator( const Int& width, const Int& height ) const override;


	virtual bool Render( const lfrt::RayGenerator& raygen, const lfrt::SampleGenerator& sampleGen, lfrt::SampleAccumulator& sampleAccum ) const override;


	bool Render( const lfrt::RayGenerator& raygen, lfrt::SampleGenerator& sampleGen, cv::Mat& result, const Int width, const Int height ) const;


public:
	const DisplayLenslet* DisplayModel = nullptr;
	cv::Mat DisplayImage = cv::Mat();
};


#endif // DISPLAYLENSLETSHOW_H