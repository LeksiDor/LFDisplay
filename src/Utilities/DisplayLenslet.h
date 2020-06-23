#ifndef DISPLAYLENSLET_H
#define DISPLAYLENSLET_H

#include "BaseTypes.h"

// Data storage for lenslet-based display.
// Display geometry:
// Observer line: (x,0,0).
// Observer plane: (x,y,0).
// Lenslet plane: (x,y,f), where f = LensletToOrigin.
// LCD plane: (x,y,d+f), where d = LensletToLCD.
class DisplayLenslet
{
public:


	bool SetLensletPositioning( const Vec2& shift, const Mat22& orientation );
	bool SetEIPositioning( const Vec2& shift, const Mat22& orientation );


	bool Save( const std::string& filepath ) const;
	bool Load( const std::string& filepath );

	const Vec2& LensletShift() const { return lensletShift; }
	const Mat22& LensletOrientation() const { return lensletOrientation; }

	const Vec2& LensletShiftInv() const { return lensletShiftInv; }
	const Mat22& LensletOrientationInv() const { return lensletOrientationInv; }

	const Vec2& EIShift() const { return eiShift; }
	const Mat22& EIOrientation() const { return eiOrientation; }

	const Vec2& EIShiftInv() const { return eiShiftInv; }
	const Mat22& EIOrientationInv() const { return eiOrientationInv; }


public:
	Real LensletToLCD; // Distance from lenslet surface to LCD.
	Real LensletToOrigin; // Distance from origin to lenslet surface.

	Vec2 SizeLCD; // Physical size of the LCD.
	Vec2i ResolutionLCD; // Resolution of the LCD in pixels.

	bool IsLensletVertical;

private:
	Vec2 lensletShift = Vec2(0,0); // Position of the lenslet center.
	Mat22 lensletOrientation = Mat22(1,0,0,1); // Size and rotation of the lenslet.
	// E.g., lenslet with index (i,j) will have central position at
	//     P = Shift + Orientation * (i,j).
	Vec2 lensletShiftInv = Vec2(0,0);
	Mat22 lensletOrientationInv = Mat22(1,0,0,1);

	Vec2 eiShift = Vec2(0,0);
	Mat22 eiOrientation = Mat22(1,0,0,1);
	Vec2 eiShiftInv = Vec2(0,0);
	Mat22 eiOrientationInv = Mat22(1,0,0,1);

};

#endif // DISPLAYLENSLET_H