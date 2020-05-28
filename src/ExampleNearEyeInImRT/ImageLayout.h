#pragma once

#include "BaseTypes.h"
#include "BaseEnums.h"
#include <vector>
#include <cmath>


namespace LFOptics
{


// Coordinate systems:
//    * Surface -- real coordinates for a corresponding physical surface (e.g., LCD, projector).
//    * Image -- image-space coordinates (usually real) of image position, from [0,width]x[0,height] range.
//    * Pixel -- image-space coordinates (usually integer) of pixel position, from [0,width-1]x[0,heigh-1] range.
class ImageLayout
{
public:
	ImageLayout() = default;

	void Resize( const Int numImages = 0 );

	const Int NumImages() const { return m_NumImages; }
	const std::vector<Int>& ImageLayer() const { return m_ImageLayer; }
	const std::vector<MinMax2Int>& ImageMinMax() const { return m_ImageMinMax; }
	const std::vector<Affine2>& ImageToSurface() const { return m_ImageToSurface; }
	const std::vector<Affine2>& SurfaceToImage() const { return m_SurfaceToImage; }

	const Int& ImageLayer( const Int imageInd ) const { return m_ImageLayer[imageInd]; }
	const MinMax2Int& ImageMinMax( const Int imageInd ) const { return m_ImageMinMax[imageInd]; }
	const Affine2& ImageToSurface( const Int imageInd ) const { return m_ImageToSurface[imageInd]; }
	const Affine2& SurfaceToImage( const Int imageInd ) const { return m_SurfaceToImage[imageInd]; }

	Int& ImageLayer( const Int imageInd ) { return m_ImageLayer[imageInd]; }
	MinMax2Int& ImageMinMax( const Int imageInd ) { return m_ImageMinMax[imageInd]; }
	Affine2& ImageToSurface( const Int imageInd ) { return m_ImageToSurface[imageInd]; }
	Affine2& SurfaceToImage( const Int imageInd ) { return m_SurfaceToImage[imageInd]; }

	Int GetNumLayers() const;
	MinMax2Int GetPixelCoverage() const;
	Vec2i GetMaxResolution() const;

	// Pixel-image transformations.
	inline static Vec2 PixelToImage( const Vec2& pixelCoord ) { return pixelCoord + Vec2( 0.5, 0.5 ); }
	inline static Vec2 PixelToImage( const Vec2i& pixelCoord ) { return Vec2(pixelCoord.x,pixelCoord.y) + Vec2(0.5,0.5); }
	inline static Vec2 ImageToPixelReal( const Vec2& imageCoord ) { return imageCoord - Vec2(0.5,0.5); }
	inline static Vec2i ImageToPixelInt( const Vec2& imageCoord ) { return Vec2i( std::lround(imageCoord.x-0.5), std::lround(imageCoord.y-0.5) ); }
	inline static Vec2 PixelToImage( const Real&  pixelCoordX, const Real&  pixelCoordY ) { return Vec2( pixelCoordX+0.5, pixelCoordY+0.5 ); }
	inline static Vec2 PixelToImage( const Int& pixelCoordX, const Int& pixelCoordY ) { return Vec2( Real(pixelCoordX)+0.5, Real(pixelCoordY)+0.5); }
	inline static Vec2 ImageToPixelReal( const Real& imageCoordX, const Real& imageCoordY ) { return Vec2( Real(imageCoordX)-0.5, Real(imageCoordY)-0.5); }
	inline static Vec2i ImageToPixelInt( const Real& imageCoordX, const Real& imageCoordY ) { return Vec2i( std::lround(imageCoordX-0.5), std::lround(imageCoordY-0.5) ); }

	// Subpixel-image transformations.
	inline static Vec2 PixelToImage( const Vec2&  pixelCoord, const Int channelInd, const SubpixelType subpixelType );
	inline static Vec2 PixelToImage( const Vec2i& pixelCoord, const Int channelInd, const SubpixelType subpixelType );
	inline static Vec2 ImageToPixelReal( const Vec2& imageCoord, const Int channelInd, const SubpixelType subpixelType );
	inline static Vec2i ImageToPixelInt( const Vec2& imageCoord, const Int channelInd, const SubpixelType subpixelType );

private:
	Int m_NumImages = 0;
	std::vector<Int> m_ImageLayer;
	std::vector<MinMax2Int> m_ImageMinMax;
	std::vector<Affine2> m_ImageToSurface;
	std::vector<Affine2> m_SurfaceToImage;
};


} // namespace LFOptics