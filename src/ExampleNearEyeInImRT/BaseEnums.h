#pragma once


namespace LFOptics
{


enum class SubpixelType
{
	Combined = 0, // All subpixels have the same coordinate (typical for projectors).
	RegularHor = 1, // Subpixels - three vertical stripes. Same-color subpixels are in same vertical column.
	DiagonalHor = 2, // Subpixels - three vertical stripes. Same-color subpixels are in same downhill diagonal.
};


enum class PixelInterpolation
{
	Neighbor, // Nearest neighbor.
	Linear, // Bilinear: c = (1-w)*c0 + w*c1, where w = lambda.
	Trapezoidal, // w = clamp( 1.5-2*lambda, 0, 1 ).
};


enum class ApertureShape
{
	Square = 0,
	Disk = 1,
};



} // namespace LFOptics