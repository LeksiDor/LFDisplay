#include "LFRendererRoutines.h"

#include "ImageLayout.h"
#include "Image2D.h"

#include "BaseInterfaces.h"

#include <cmath>
#include <iomanip>
#include <sstream>

#include <thread>



namespace LFOptics
{



Int& LFRendererRoutines::NumberOfThreads()
{
	static Int NumThreads = 8;
	return NumThreads;
}


void LFRendererRoutines::Raytrace( Image2D& resultImage, const IRayGenerator& rayGenerator, const IRayColorizer& rayProcessor, const MinMax2Int& imageRange, const Int numSamplesSqrt, const Affine2& imageToSurface, const Affine3& rayTransform )
{
	RaytraceInThread( &resultImage, &rayGenerator, &rayProcessor, imageRange, numSamplesSqrt, imageToSurface, rayTransform );
}


void LFRendererRoutines::RaytraceMultithreaded( Image2D& resultImage, const IRayGenerator& rayGenerator, const IRayColorizer& rayProcessor, const MinMax2Int& imageRange, const Int numSamplesSqrt, const Affine2& imageToSurface, const Affine3& rayTransform )
{
	const Int numthreads = NumberOfThreads();

	if ( numthreads <= 1 )
	{
		Raytrace( resultImage, rayGenerator, rayProcessor, imageRange, numSamplesSqrt, imageToSurface, rayTransform );
		return;
	}

	if ( numSamplesSqrt <= 0 || numSamplesSqrt == TypeNaN<Int>() )
		return;
	if ( !imageRange.isValid() )
		return;
	if ( imageRange.min.x < 0 || imageRange.min.y < 0 )
		return;
	if ( resultImage.Width() <= imageRange.max.x || resultImage.Height() <= imageRange.max.y )
		return;

	std::vector<MinMax2Int> imageRangePerThread(numthreads);
	const Int totalRows = imageRange.max.y - imageRange.min.y + 1;
	const Int averageRows = (totalRows + numthreads - 1) / numthreads;
	for ( Int i = 0; i < numthreads; ++i )
	{
		imageRangePerThread[i] = imageRange;
		imageRangePerThread[i].min.y = imageRange.min.y + i*averageRows;
		imageRangePerThread[i].max.y = imageRange.min.y + (i+1)*averageRows - 1;
	}
	imageRangePerThread[numthreads-1].max.y = imageRange.max.y;

	std::vector<std::thread> threads(numthreads);

	for ( Int i = 0; i < numthreads; ++i )
	{
		const MinMax2Int& imageRangeInThread = imageRangePerThread[i];
		threads[i] = std::thread( RaytraceInThread, &resultImage, &rayGenerator, &rayProcessor, imageRangeInThread, numSamplesSqrt, imageToSurface, rayTransform );
	}

	for ( Int i = 0; i < numthreads; ++i )
	{
		threads[i].join();
	}
}



void LFRendererRoutines::Raytrace( Image2D& resultImage, const IRayGeneratorOnImageMulti& rayGenerator, const IRayColorizer& rayProcessor, const MinMax2Int& imageRange, const bool isAverageColor, const Affine2& imageToSurface, const Affine3& rayTransform)
{
	RaytraceInThread_RayGeneratorOnImageMulti( &resultImage, &rayGenerator, &rayProcessor, imageRange, isAverageColor, imageToSurface, rayTransform );
}


void LFRendererRoutines::RaytraceMultithreaded( Image2D& resultImage, const IRayGeneratorOnImageMulti& rayGenerator, const IRayColorizer& rayProcessor, const MinMax2Int& imageRange, const bool isAverageColor, const Affine2& imageToSurface, const Affine3& rayTransform )
{
	const Int numthreads = NumberOfThreads();

	if (numthreads <= 1)
	{
		RaytraceInThread_RayGeneratorOnImageMulti( &resultImage, &rayGenerator, &rayProcessor, imageRange, isAverageColor, imageToSurface, rayTransform );
		return;
	}

	if (!imageRange.isValid())
		return;
	if (imageRange.min.x < 0 || imageRange.min.y < 0)
		return;
	if (resultImage.Width() <= imageRange.max.x || resultImage.Height() <= imageRange.max.y)
		return;

	std::vector<MinMax2Int> imageRangePerThread(numthreads);
	const Int totalRows = imageRange.max.y - imageRange.min.y + 1;
	const Int averageRows = (totalRows + numthreads - 1) / numthreads;
	for (Int i = 0; i < numthreads; ++i)
	{
		imageRangePerThread[i] = imageRange;
		imageRangePerThread[i].min.y = imageRange.min.y + i * averageRows;
		imageRangePerThread[i].max.y = imageRange.min.y + (i + 1) * averageRows - 1;
	}
	imageRangePerThread[numthreads - 1].max.y = imageRange.max.y;

	std::vector<std::thread> threads(numthreads);

	for (Int i = 0; i < numthreads; ++i)
	{
		const MinMax2Int& imageRangeInThread = imageRangePerThread[i];
		threads[i] = std::thread( RaytraceInThread_RayGeneratorOnImageMulti, &resultImage, &rayGenerator, &rayProcessor, imageRangeInThread, isAverageColor, imageToSurface, rayTransform );
	}

	for (Int i = 0; i < numthreads; ++i)
	{
		threads[i].join();
	}
}


std::string LFRendererRoutines::GenerateDirPath( const std::string& directorypath, const Int itemInd, const std::string& extension )
{
	std::stringstream stringstream;
	stringstream << directorypath << std::setfill('0') << std::setw(4) << itemInd << "." << extension;
	return stringstream.str();
}


void LFRendererRoutines::RaytraceInThread( Image2D* resultImage, const IRayGenerator* rayGenerator, const IRayColorizer* rayProcessor, const MinMax2Int imageRange, const Int numSamplesSqrt, const Affine2 imageToSurface, const Affine3 rayTransform )
{
	if ( numSamplesSqrt <= 0 || numSamplesSqrt == TypeNaN<Int>() )
		return;
	if ( !imageRange.isValid() )
		return;
	if ( imageRange.min.x < 0 || imageRange.min.y < 0 )
		return;
	if ( resultImage->Width() <= imageRange.max.x || resultImage->Height() <= imageRange.max.y )
		return;

	const Real invNumSamples = Real(1.0) / Real(numSamplesSqrt*numSamplesSqrt);

	for ( Int x = imageRange.min.x; x <= imageRange.max.x; ++x )
	{
		for ( Int y = imageRange.min.y; y <= imageRange.max.y; ++y )
		{
			Vec4 color = Vec4(0,0,0,0);
			for ( Int i = 0; i < numSamplesSqrt; ++i )
			{
				for ( Int j = 0; j < numSamplesSqrt; ++j )
				{
					const Vec2 imageCenter = ImageLayout::PixelToImage(x,y);
					const Vec2 samplingShift = Vec2( (Real(i)+0.5)/Real(numSamplesSqrt)-0.5, (Real(j)+0.5)/Real(numSamplesSqrt)-0.5 );
					const Vec2 imageCoord = imageCenter + samplingShift;
					const Vec2 surfaceCoord = imageToSurface * imageCoord;
					const Ray3 rayLocal = rayGenerator->GenerateRay( surfaceCoord );
					const Ray3 rayGlobal = rayTransform * rayLocal;
					color = color + rayProcessor->ProcessRay( rayGlobal );
				}
			}
			color = color * invNumSamples;
			resultImage->Set( x, y, color );
		}
	}
}



void LFRendererRoutines::RaytraceInThread_RayGeneratorOnImageMulti( Image2D* resultImage, const IRayGeneratorOnImageMulti* rayGenerator, const IRayColorizer* rayProcessor, const MinMax2Int imageRange, const bool isAverageColor, const Affine2 imageToSurface, const Affine3 rayTransform )
{
	if (!imageRange.isValid())
		return;
	if (imageRange.min.x < 0 || imageRange.min.y < 0)
		return;
	if (resultImage->Width() <= imageRange.max.x || resultImage->Height() <= imageRange.max.y)
		return;

	const Int numSamples = rayGenerator->NumSamples();
	if (numSamples <= 0)
		return;

	std::vector<Vec4> colorWeights(numSamples);
	std::vector<Ray3> rays(numSamples);

	for (Int x = imageRange.min.x; x <= imageRange.max.x; ++x)
	{
		for (Int y = imageRange.min.y; y <= imageRange.max.y; ++y)
		{
			Vec4 color = Vec4(0,0,0,0);
			Vec4 sumWeights = Vec4(0,0,0,0);
			rayGenerator->GenerateRays( colorWeights, rays, Vec2i(x,y) );
			for ( Int sampleInd = 0; sampleInd < numSamples; ++sampleInd )
			{
				const Vec4& weight = colorWeights[sampleInd];
				const Ray3& rayLocal = rays[sampleInd];
				const Ray3 ray = rayTransform * rayLocal;
				if ( weight.x == 0 && weight.y == 0 && weight.z == 0 && weight.w == 0 )
					continue;
				const Vec4 processedColor = rayProcessor->ProcessRay(ray);
				color = color + weight*processedColor;
				sumWeights = sumWeights + weight;
			}
			if (isAverageColor)
			{
				color.x = (sumWeights.x != 0) ? color.x / sumWeights.x : 0.0;
				color.y = (sumWeights.y != 0) ? color.y / sumWeights.y : 0.0;
				color.z = (sumWeights.z != 0) ? color.z / sumWeights.z : 0.0;
				color.w = (sumWeights.w != 0) ? color.w / sumWeights.w : 0.0;
			}
			resultImage->Set( x, y, color );
		}
	}
}



} // namespace LFOptics