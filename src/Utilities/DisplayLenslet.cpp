#include "DisplayLenslet.h"


bool DisplayLenslet::SetLensletPositioning( const Vec2& shift, const Mat22& orientation )
{
	bool isInvertible = false;
	Mat22 inverse = orientation.inv( 0, &isInvertible );
	if ( !isInvertible )
		return false;

	lensletShift = shift;
	lensletOrientation = orientation;

	lensletShiftInv = -inverse*shift;
	lensletOrientationInv = inverse;

	return true;
}


bool DisplayLenslet::SetEIPositioning(const Vec2& shift, const Mat22& orientation)
{
	bool isInvertible = false;
	Mat22 inverse = orientation.inv( 0, &isInvertible );
	if ( !isInvertible )
		return false;

	eiShift = shift;
	eiOrientation = orientation;

	eiShiftInv = -inverse*shift;
	eiOrientationInv = inverse;

	return true;
}


bool DisplayLenslet::Save( const std::string& filepath ) const
{
	cv::FileStorage fs( filepath, cv::FileStorage::WRITE );
	if ( !fs.isOpened() )
		return false;

	fs << "lenslettolcd" << this->LensletToLCD;
	fs << "lenslettoorigin" << this->LensletToOrigin;
	fs << "sizelcd" << this->SizeLCD;
	fs << "resolutionlcd" << this->ResolutionLCD;
	fs << "islensletvertical" << this->IsLensletVertical;

	fs << "lensletshift" << this->lensletShift;
	fs << "lensletorientation" << this->lensletOrientation;

	fs << "eishift" << this->eiShift;
	fs << "eiorientation" << this->eiOrientation;

	fs.release();
	return true;
}


bool DisplayLenslet::Load( const std::string& filepath )
{
	cv:: FileStorage fs( filepath, cv::FileStorage::READ );
	if ( !fs.isOpened() )
		return false;

	fs["lenslettolcd"] >> this->LensletToLCD;
	fs["lenslettoorigin"] >> this->LensletToOrigin;
	fs["sizelcd"] >> this->SizeLCD;
	fs["resolutionlcd"] >> this->ResolutionLCD;
	fs["islensletvertical"] >> this->IsLensletVertical;

	Vec2 shift;
	Mat22 orientation;

	fs["lensletshift"] >> shift;
	fs["lensletorientation"] >> orientation;
	SetLensletPositioning( shift, orientation );

	fs["eishift"] >> shift;
	fs["eiorientation"] >> orientation;
	SetEIPositioning( shift, orientation );

	fs.release();
	return true;

	return false;
}
