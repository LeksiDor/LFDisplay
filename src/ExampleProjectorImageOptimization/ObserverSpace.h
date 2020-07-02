#ifndef OBSERVERSPACE_H
#define OBSERVERSPACE_H

#include "BaseTypes.h"


class ObserverSpace
{
public:
	ObserverSpace() = default;

	Int NumPositions() const { return NumViewers; }
	Vec3 Position( const Int& index ) const { return ViewerStart + Real(index)*ViewerStep; }
	Real Weight( const Int& index ) const { return 1.0;}
	Real SumWeights() const { return NumViewers; }

public:
	Vec3 ViewerStart = Vec3( 0, 0, 0 );
	Vec3 ViewerStep = Vec3( 0, 0, 0 );
	Int NumViewers = 1;
};


#endif // OBSERVERSPACE_H