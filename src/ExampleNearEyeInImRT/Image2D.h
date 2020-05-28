#pragma once

#include "BaseTypes.h"
#include "BaseEnums.h"
#include <string>
#include <vector>


namespace LFOptics
{


class Image2D
{
public:
	Image2D();
	Image2D( const Int width, const Int height );

	bool Set( const Int x, const Int y, const Vec4& value );

	const Vec4 at( const Int x, const Int y ) const;
	const Vec4 at( const Real x, const Real y, const PixelInterpolation& interpolation ) const;

	void Resize( const Int width, const Int height );

	MinMax2Int ImageRange() const { return MinMax2Int( 0, 0, Width()-1, Height()-1 ); }

	Int Width() const { return width; }
	Int Height() const { return height; }

	bool Load( const std::string& filepath );
	bool Save( const std::string& filepath ) const;

	bool LoadAsEXR(const std::string& filepath);
	bool SaveAsPPM(const std::string& filepath) const;
	bool SaveAsEXR(const std::string& filepath) const;

	bool SaveAsPNG(const std::string& filepath) const { return SaveQuantized(filepath, 0); }
	bool SaveAsBMP(const std::string& filepath) const { return SaveQuantized(filepath, 1); }
	bool SaveAsTGA(const std::string& filepath) const { return SaveQuantized(filepath, 2); }
	bool SaveAsJPG(const std::string& filepath) const { return SaveQuantized(filepath, 3); }

	void Reset();
	void Clear( const Vec4& color = Vec4( RealNaN, RealNaN, RealNaN, RealNaN ) );

private:
	// 0 - png, 1 - bmp, 2 - tga, 3 - jpg.
	bool SaveQuantized( const std::string& filepath, const Int& type ) const;

private:
	std::vector<Vec4> data;
	Int width;
	Int height;
};

} // namespace LFOptics