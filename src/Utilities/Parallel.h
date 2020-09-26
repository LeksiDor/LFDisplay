#ifndef UTILITIES_PARALLEL_H
#define UTILITIES_PARALLEL_H

#include "BaseTypes.h"

void CVParallel2D( const Int& width, const Int& height, std::function<void(const Int x, const Int y)> functor )
{
	cv::parallel_for_(
		cv::Range( 0, width*height ),
		cv::ParallelLoopBodyLambdaWrapper( [&]( const cv::Range& range )
			{
				for ( int pixelInd = range.start; pixelInd < range.end; ++pixelInd )
				{
					functor( pixelInd % width, pixelInd / width );
				}
			} )
	);
}

#endif // UTILITIES_PARALLEL_H