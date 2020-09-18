#ifndef DISPLAYPROJECTORSSHOW_H
#define DISPLAYPROJECTORSSHOW_H

#include "BaseTypes.h"
#include "LFRayTracer.h"

#include <opencv2/opencv.hpp>

class DiffuserModel;
class DisplayProjectorAligned;


class DisplayProjectorsShow
	: public lfrt::LFRayTracer
{
public:

	using Color = cv::Vec3f;

	DisplayProjectorsShow( const DisplayProjectorAligned* displayModel );

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

public:
	std::vector<cv::Mat> ProjectorImages;
	std::vector<Vec3> ProjectorPositions;

private:
	const DisplayProjectorAligned* m_DisplayModel = nullptr;
	std::shared_ptr<DiffuserModel> m_DiffuserModel = nullptr;
};


#endif // DISPLAYPROJECTORSSHOW_H