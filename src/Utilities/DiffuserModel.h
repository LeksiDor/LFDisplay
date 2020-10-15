#ifndef UTILITIES_DIFFUSERMODEL_H
#define UTILITIES_DIFFUSERMODEL_H

#include "BaseTypes.h"


// Abstract class for diffuser model.
// Does not consider the geometrical shape of provided diffuser.
// All calculations are done in local diffuser coordinates,
// where point (0,0,0) corresponds to the ray intersection point,
// diffuser axis of anisotropy is (1,0,0),
// and diffuser normal is (0,0,1).
class DiffuserModel
{
public:
	virtual ~DiffuserModel() = default;

	// Line is parameterized as linePos = lineOri + lambda*lineDir.
	// Returns parameter 'lambda' which corresponds to the maximal refracted ray intensity.
	virtual Real FindMaxOnLine( const Vec3& rayOri, const Vec3& lineOri, const Vec3& lineDir = Vec3(1,0,0) ) const = 0;

	// Returns intensity of refracted light ray which starts at 'pointA' and terminates at 'pointB'.
	virtual Real Diffusion( const Vec3& pointA, const Vec3& pointB ) const = 0;
};


#endif // UTILITIES_DIFFUSERMODEL_H