#ifndef DISPLAYPROJECTORSSHOW_H
#define DISPLAYPROJECTORSSHOW_H

#include "BaseTypes.h"
#include "LFRayTracer.h"

#include <opencv2/opencv.hpp>

class DisplayProjectorAligned;

class DisplayProjectorsShow
	: public lfrt::LFRayTracer
{
public:

	using Color = cv::Vec3f;

	DisplayProjectorsShow( const DisplayProjectorAligned* displayModel = nullptr );

	virtual ~DisplayProjectorsShow() = default;

	// Filepath is location of the folder with projector images.
	// Image names must be formatted as "xxxx.exr".
	virtual bool LoadScene( const std::string& filepath ) override;

	// Returns simple pinhole camera.
	virtual lfrt::RayGenerator* CreateDefaultRayGenerator( const Int& width, const Int& height ) const override;

	// Returns uniform sample generator.
	virtual lfrt::SampleGenerator* CreateDefaultSampleGenerator( const Int& width, const Int& height ) const override;

	// Returns OpenCV-based sample accum.
	virtual lfrt::SampleAccumulator* CreateDefaultSampleAccumulator( const Int& width, const Int& height ) const override;

	virtual bool Render( const lfrt::RayGenerator& raygen, const lfrt::SampleGenerator& sampleGen, lfrt::SampleAccumulator& sampleAccum ) const override;


private:
	static Real RhoConical( const Real& dx, const Real& dy, const Real& dz );
	static Real EtaConical( const Real& dx, const Real& dy, const Real& dz );
	static Real RhoLinear( const Real& dx, const Real& dy, const Real& dz );
	static Real EtaLinear( const Real& dx, const Real& dy, const Real& dz );

public:
	const DisplayProjectorAligned* DisplayModel = nullptr;
	std::vector<cv::Mat> ProjectorImages;
	std::vector<Vec3> ProjectorPositions;
};


#endif // DISPLAYPROJECTORSSHOW_H