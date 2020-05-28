#pragma once

#include "BaseTypes.h"

#include <string>



namespace LFOptics
{


class Image2D;
class IRayColorizer;
class IRayGenerator;
class IRayGeneratorOnImageMulti;

class LFRendererRoutines
{	
public:

	static Int& NumberOfThreads();

	static void Raytrace(
		Image2D& resultImage,
		const IRayGenerator& rayGenerator,
		const IRayColorizer& rayProcessor,
		const MinMax2Int& imageRange,
		const Int numSamplesSqrt = 1,
		const Affine2& imageToSurface = Affine2::identity(),
		const Affine3& rayTransform = Affine3::identity()
	);

	static void RaytraceMultithreaded(
		Image2D& resultImage,
		const IRayGenerator& rayGenerator,
		const IRayColorizer& rayProcessor,
		const MinMax2Int& imageRange,
		const Int numSamplesSqrt = 1,
		const Affine2& imageToSurface = Affine2::identity(),
		const Affine3& rayTransform = Affine3::identity()
	);


	static void Raytrace(
		Image2D& resultImage,
		const IRayGeneratorOnImageMulti& rayGenerator,
		const IRayColorizer& rayProcessor,
		const MinMax2Int& imageRange,
		const bool isAverageColor = true,
		const Affine2& imageToSurface = Affine2::identity(),
		const Affine3& rayTransform = Affine3::identity()
	);

	static void RaytraceMultithreaded(
		Image2D& resultImage,
		const IRayGeneratorOnImageMulti& rayGenerator,
		const IRayColorizer& rayProcessor,
		const MinMax2Int& imageRange,
		const bool isAverageColor = true,
		const Affine2& imageToSurface = Affine2::identity(),
		const Affine3& rayTransform = Affine3::identity()
	);


	static std::string GenerateDirPath( const std::string& directorypath, const Int itemInd, const std::string& extension = "exr" );

private:
	static void RaytraceInThread(
		Image2D* resultImage,
		const IRayGenerator* rayGenerator,
		const IRayColorizer* rayProcessor,
		const MinMax2Int imageRange,
		const Int numSamplesSqrt,
		const Affine2 imageToSurface,
		const Affine3 rayTransform
	);

	static void RaytraceInThread_RayGeneratorOnImageMulti(
		Image2D* resultImage,
		const IRayGeneratorOnImageMulti* rayGenerator,
		const IRayColorizer* rayProcessor,
		const MinMax2Int imageRange,
		const bool isAverageColor,
		const Affine2 imageToSurface,
		const Affine3 rayTransform
	);
};

} // namespace LFOptics