#include "ImageAnalysis.h"

#include "Image2D.h"

#include <algorithm>


namespace LFOptics
{


bool ImageAnalysis::ImageDiff( const Image2D& left, const Image2D& right, Image2D& result, const bool isAbsoluteDiff )
{
	const Int width = left.Width();
	const Int height = left.Height();

	if (width <= 0 || height <= 0)
		return false;
	if ( right.Width() != width || right.Height() != height )
		return false;

	if ( result.Width() != width || result.Height() != height )
		result.Resize(width,height);

	for ( Int x = 0; x < width; ++x )
	{
		for ( Int y = 0; y < height; ++y )
		{
			const Vec4 diff = left.at(x,y) - right.at(x,y);
			const Vec4 value = isAbsoluteDiff ? Vec4(abs(diff.x),abs(diff.y),abs(diff.z),abs(diff.w)) : diff;
			result.Set( x, y, value );
		}
	}

	return true;
}


Vec4 ImageAnalysis::ImageMax(const Image2D& image)
{
	Vec4 result = Vec4(RealMin,RealMin,RealMin,RealMin);
	const Int width = image.Width();
	const Int height = image.Height();

	for ( Int x = 0; x < width; ++x )
	{
		for ( Int y = 0; y < height; ++y )
		{
			const Vec4& color = image.at(x,y);
			result = Vec4(
				std::max<Real>(result.x,color.x),
				std::max<Real>(result.y,color.y),
				std::max<Real>(result.z,color.z),
				std::max<Real>(result.w,color.w) );
		}
	}

	return result;
}


Vec4 ImageAnalysis::ImageMSE(const Image2D& left, const Image2D& right, const Mat44& transform)
{
	const Int width = left.Width();
	const Int height = left.Height();

	if (width <= 0 || height <= 0)
		return Vec4(RealNaN,RealNaN,RealNaN,RealNaN);
	if ( right.Width() != width || right.Height() != height )
		return Vec4(RealNaN,RealNaN,RealNaN,RealNaN);

	Vec4 sum = Vec4(0,0,0,0);

	for ( Int x = 0; x < width; ++x )
	{
		Vec4 sum_local = Vec4(0,0,0,0);
		for ( Int y = 0; y < height; ++y )
		{
			const Vec4 diff = left.at(x,y) - right.at(x,y);
			const Vec4 diff_transformed = transform * diff;
			const Vec4 diff2_transformed = diff_transformed*diff_transformed;
			sum_local = sum_local + diff2_transformed;
		}
		sum_local = sum_local / Real(height);
		sum = sum + sum_local;
	}
	sum = sum / Real(width);

	return sum;
}


Vec4 ImageAnalysis::ImageSSIM(const Image2D& left, const Image2D& right, Image2D& ssim, const Mat44& transform, const Int windowRadius, const Real invPixelSigmaSqr)
{
	const Int width = left.Width();
	const Int height = left.Height();
	if ( width <= 0 || height <= 0 )
		return TypeNaN<Vec4>();
	if ( right.Width() != width || right.Height() != height )
		return TypeNaN<Vec4>();

	if ( ssim.Width() != width || ssim.Height() != height )
		ssim.Resize(width,height);

	std::vector<Real> weights(windowRadius + 1);
	for (Int i = 0; i <= windowRadius; ++i)
	{
		weights[i] = std::exp( -Real(i)*Real(i)*invPixelSigmaSqr );
	}

	const Real L = 1.0;
	const Real k1 = 0.01;
	const Real k2 = 0.03;
	const Real c1 = k1*k1*L*L;
	const Real c2 = k2*k2*L*L;

	for ( Int x = 0; x < width; ++x )
	{
		for ( Int y = 0; y < height; ++y )
		{
			Vec4 E_X  = Vec4(0,0,0,0);
			Vec4 E_Y  = Vec4(0,0,0,0);
			Vec4 E_XX = Vec4(0,0,0,0);
			Vec4 E_YY = Vec4(0,0,0,0);
			Vec4 E_XY = Vec4(0,0,0,0);
			Real sum_w = 0;

			const Int radminx = std::min<Int>( windowRadius, x );
			const Int radminy = std::min<Int>( windowRadius, y );
			const Int radmaxx = std::min<Int>( windowRadius, width-x-1 );
			const Int radmaxy = std::min<Int>( windowRadius, height-y-1 );

			for ( Int radx = radminx; radx <= radmaxx; ++radx )
			{
				for ( Int rady = radminy; rady <= radmaxy; ++rady )
				{
					const Real w = weights[abs(radx)]*weights[abs(rady)];
					sum_w += w;
					const Vec2i pixcoord = Vec2i(x+radx,y+rady);
					const Vec4 val_X = transform * left.at(pixcoord.x,pixcoord.y);
					const Vec4 val_Y = transform * right.at(pixcoord.x,pixcoord.y);
					E_X  = E_X  + w*val_X;
					E_Y  = E_Y  + w*val_Y;
					E_XX = E_XX + w*val_X*val_X;
					E_YY = E_YY + w*val_Y*val_Y;
					E_XY = E_XY + w*val_X*val_Y;
				}
			}

			if (sum_w > 0)
			{
				E_X  = E_X  / sum_w;
				E_Y  = E_Y  / sum_w;
				E_XX = E_XX / sum_w;
				E_YY = E_YY / sum_w;
				E_XY = E_XY / sum_w;
			}

			const Vec4 sigma_XX = E_XX - E_X*E_X;
			const Vec4 sigma_YY = E_YY - E_Y*E_Y;
			const Vec4 sigma_XY = E_XY - E_X*E_Y;
			const Vec4 nom1 = 2.0*E_X*E_Y + c1*Vec4(1,1,1,1);
			const Vec4 nom2 = 2.0*sigma_XY + c2*Vec4(1,1,1,1);
			const Vec4 denom1 = E_X*E_X + E_Y*E_Y + c1*Vec4(1,1,1,1);
			const Vec4 denom2 = sigma_XX + sigma_YY + c2*Vec4(1,1,1,1);
			const Vec4 ssim_val = nom1*nom2/(denom1*denom2);
			ssim.Set( x, y, ssim_val );
		}
	}

	Vec4 sum = Vec4(0,0,0,0);
	for ( Int x = 0; x < width; ++x )
	{
		Vec4 sum_local = Vec4(0,0,0,0);
		for ( Int y = 0; y < height; ++y )
		{
			const Vec4 value = ssim.at(x,y);
			sum_local = sum_local + value;
		}
		sum_local = sum_local / Real(height);
		sum = sum + sum_local;
	}
	sum = sum / Real(width);

	return sum;
}


void ImageAnalysis::ImageHistogram(const Image2D& image, const Vec4& histmin, const Vec4& histmax, const Int& numBins, std::vector<Vec4>& hist, const bool isRelative, const Mat44& transform)
{
	if ( numBins <= 0 )
		return;
	if ( hist.size() != numBins )
		hist.resize(numBins);

	const Int width = image.Width();
	const Int height = image.Height();

	if (width <= 0 || height <= 0)
		return;

	std::vector<Vec4i> hist_abs(numBins);
	for ( Int i = 0; i < numBins; ++i )
		hist_abs[i] = Vec4i(0,0,0,0);

	for ( Int x = 0; x < width; ++x )
	{
		for ( Int y = 0; y < height; ++y )
		{
			const Vec4& color = image.at(x,y);
			const Vec4 color_t = transform*color;
			const Vec4 binind_real = (color_t-histmin)/(histmax-histmin)*Real(numBins);
			const Vec4i binind = Vec4i(
				(Int)std::floor(binind_real.x),
				(Int)std::floor(binind_real.y),
				(Int)std::floor(binind_real.z),
				(Int)std::floor(binind_real.w) );
			if ( binind.x >= 0 && binind.x < numBins )
				hist_abs[binind.x].x += 1;
			if ( binind.y >= 0 && binind.y < numBins )
				hist_abs[binind.y].y += 1;
			if ( binind.z >= 0 && binind.z < numBins )
				hist_abs[binind.z].z += 1;
			if ( binind.w >= 0 && binind.w < numBins )
				hist_abs[binind.w].w += 1;
		}
	}

	for ( Int i = 0; i < numBins; ++i )
	{
		hist[i] = hist_abs[i].toType<Real>();
		if ( isRelative )
		{
			hist[i] = hist[i] / (Real(width)*Real(height));
		}
	}
}


} // namespace LFOptics