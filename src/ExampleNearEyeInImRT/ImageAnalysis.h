#pragma once

#include "BaseTypes.h"

#include <vector>


namespace LFOptics
{

class Image2D;

class ImageAnalysis
{
public:

	static bool ImageDiff( const Image2D& left, const Image2D& right, Image2D& result, const bool isAbsoluteDiff = false );

	static Vec4 ImageMax( const Image2D& image );

	static Vec4 ImageMSE( const Image2D& left, const Image2D& right, const Mat44& transform = Mat44::identity() );

	static Vec4 ImageSSIM( const Image2D& left, const Image2D& right, Image2D& ssim, const Mat44& transform = Mat44::identity(), const Int windowRadius = 5, const Real invPixelSigmaSqr = 1.0/1.5 );

	static void ImageHistogram( const Image2D& image, const Vec4& histmin, const Vec4& histmax, const Int& numBins, std::vector<Vec4>& hist, const bool isRelative = true, const Mat44& transform = Mat44::identity() );
};

} // namespace LFOptics