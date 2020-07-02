#ifndef DISPLAYPROJECTORITERATIONS_H
#define DISPLAYPROJECTORITERATIONS_H

#include "BaseTypes.h"


class DisplayProjectorAligned;
class ObserverSpace;


class DisplayProjectorIterations
{
public:

	DisplayProjectorIterations(
		const DisplayProjectorAligned* displayModel = nullptr,
		const ObserverSpace* viewerSpace = nullptr );

	bool Iterate(
		const std::vector<cv::Mat>& groundtrue, // Ground-true images for each position in the observer space.
		const std::vector<cv::Mat>& zeroIteration, // Initial set of projector images.
		std::vector< std::vector<cv::Mat> >& iterations, // Array of set of projector images; don't initialize it.
		const Int numIterations ) const;

public:
	const DisplayProjectorAligned* DisplayModel = nullptr;
	const ObserverSpace* ViewerSpace = nullptr;

};


#endif // DISPLAYPROJECTORITERATIONS_H