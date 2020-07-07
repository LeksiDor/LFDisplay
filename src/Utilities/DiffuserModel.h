#ifndef DIFFUSERMODEL_H
#define DIFFUSERMODEL_H

#include "BaseTypes.h"

class DisplayProjectorAligned;


class DiffuserModel
{
public:
	virtual ~DiffuserModel() = default;

	// Viewer line is parameterized as (x,0,0).
	// Return value: X such that (X,0,0) gives the maximum intensity for the specified ray.
	virtual Real FindMaxOnViewerLine( const Vec3& rayOrigin, const Vec3& intersection ) const = 0;

	// Returns intensity multiplier for the ray with specified origin,
	// screen intersection point, and final destination.
	virtual Real RefractedIntensity( const Vec3& rayOrigin, const Vec3& intersection, const Vec3& destination ) const = 0;

public:
	static DiffuserModel* Create( const DisplayProjectorAligned& displayModel );
};


#endif // DIFFUSERMODEL_H