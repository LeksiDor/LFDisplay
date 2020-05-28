#include "ImageLayout.h"

#include <fstream>
#include <iomanip>
#include <algorithm>



namespace LFOptics
{


void ImageLayout::Resize( const Int numImages )
{
	m_ImageLayer.clear();
	m_ImageMinMax.clear();
	m_ImageToSurface.clear();
	m_SurfaceToImage.clear();
	m_NumImages = numImages;
	if ( numImages == 0 )
		return;
	m_ImageLayer.resize( numImages, IntNaN );
	m_ImageMinMax.resize( numImages, MinMax2Int(IntMax,IntMax,IntMin,IntMin) );
	m_ImageToSurface.resize( numImages, Affine2(
		Mat22(RealNaN,RealNaN,RealNaN,RealNaN),
		Vec2(RealNaN,RealNaN) ) );
	m_SurfaceToImage.resize( numImages, Affine2(
		Mat22(RealNaN,RealNaN,RealNaN,RealNaN),
		Vec2(RealNaN,RealNaN) ) );
}


Int ImageLayout::GetNumLayers() const
{
	Int maxLayerInd = -1;
	for ( Int imageInd = 0; imageInd < m_NumImages; ++imageInd )
	{
		const Int layerInd = m_ImageLayer[imageInd];
		if ( layerInd == TypeNaN<Int>() )
			continue;
		maxLayerInd = std::max<Int>( maxLayerInd, layerInd );
	}
	return maxLayerInd+1;
}


MinMax2Int ImageLayout::GetPixelCoverage() const
{
	MinMax2Int minmax;
	for ( Int imageInd = 0; imageInd < m_NumImages; ++imageInd )
	{
		minmax = minmax.add( m_ImageMinMax[imageInd] );
	}
	return minmax;
}


Vec2i ImageLayout::GetMaxResolution() const
{
	const MinMax2Int pixelCoverage = GetPixelCoverage();
	if ( pixelCoverage.isValid() )
		return Vec2i( pixelCoverage.max.x + 1, pixelCoverage.max.y + 1 );
	else
		return Vec2i( 0, 0 );
}


inline Vec2 ImageLayout::PixelToImage( const Vec2& pixelCoord, const Int channelInd, const SubpixelType subpixelType )
{
	const Vec2 center = pixelCoord + Vec2(0.5,0.5);
	switch ( subpixelType )
	{
	case SubpixelType::Combined:
		return center;
		break;
	case SubpixelType::RegularHor:
		return center + Vec2( Real(channelInd-1)/3.0, 0 );
		break;
	case SubpixelType::DiagonalHor:
		return center + Vec2( Real(((channelInd+Int(pixelCoord.y+0.5))%3)-1)/3.0, 0 );
		//return TypeNaN<Vec2>();
		break;
	default:
		return TypeNaN<Vec2>();
		break;
	}
}


inline Vec2 ImageLayout::PixelToImage( const Vec2i& pixelCoord, const Int channelInd, const SubpixelType subpixelType )
{
	const Vec2 center = Vec2(pixelCoord.x,pixelCoord.y) + Vec2(0.5,0.5);
	switch ( subpixelType )
	{
	case SubpixelType::Combined:
		return center;
		break;
	case SubpixelType::RegularHor:
		return center + Vec2( Real(channelInd-1)/3.0, 0 );
		break;
	case SubpixelType::DiagonalHor:
		return center + Vec2( Real(((channelInd+pixelCoord.y)%3)-1)/3.0, 0 );
		break;
	default:
		return TypeNaN<Vec2>();
		break;
	}
}


inline Vec2 ImageLayout::ImageToPixelReal( const Vec2& imageCoord, const Int channelInd, const SubpixelType subpixelType )
{
	const Vec2 pixelCoord = imageCoord - Vec2(0.5,0.5);
	switch ( subpixelType )
	{
	case SubpixelType::Combined:
		return pixelCoord;
		break;
	case SubpixelType::RegularHor:
		return pixelCoord - Vec2( (channelInd-1)/3.0, 0 );
		break;
	case SubpixelType::DiagonalHor:
		return pixelCoord - Vec2( (((channelInd+Int(pixelCoord.y+0.5))%3)-1)/3.0, 0 );
		break;
	default:
		return TypeNaN<Vec2>();
		break;
	}
}


inline Vec2i ImageLayout::ImageToPixelInt( const Vec2& imageCoord, const Int channelInd, const SubpixelType subpixelType )
{
	Vec2 pixelCoord = imageCoord - Vec2(0.5,0.5);
	switch ( subpixelType )
	{
	case SubpixelType::Combined:
		//return pixelCoord;
		break;
	case SubpixelType::RegularHor:
		pixelCoord = pixelCoord - Vec2( (channelInd-1)/3.0, 0 );
		break;
	case SubpixelType::DiagonalHor:
		pixelCoord = pixelCoord - Vec2( (((channelInd+Int(pixelCoord.y+0.5))%3)-1)/3.0, 0 );
		break;
	default:
		return TypeNaN<Vec2i>();
		break;
	}
	return Vec2i( std::lround(pixelCoord.x), std::lround(pixelCoord.y) );
}


} // namespace LFOptics