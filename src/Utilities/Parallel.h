#ifndef UTILITIES_PARALLEL_H
#define UTILITIES_PARALLEL_H

#include "BaseTypes.h"

void CVParallel2D(
	const Int& width, const Int& height,
	std::function<void(const Int x, const Int y)> functor );


#endif // UTILITIES_PARALLEL_H