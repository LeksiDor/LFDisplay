#ifndef DISPLAYPROJECTORSOPTIMIZATION_H
#define DISPLAYPROJECTORSOPTIMIZATION_H

#include "BaseTypes.h"


class DiffuserModel;
class DisplayProjectorAligned;
class ObserverSpace;


class DisplayProjectorsOptimization
{
public:
	using Color = cv::Vec3f;

public:

	DisplayProjectorsOptimization(
		const DisplayProjectorAligned* displayModel,
		const ObserverSpace* viewerSpace );

	bool Iterate(
		const std::vector<cv::Mat>& groundtrue, // Ground-true images for each position in the observer space.
		const std::vector<cv::Mat>& zeroIteration, // Initial set of projector images.
		std::vector< std::vector<cv::Mat> >& iterations, // Array of set of projector images; don't initialize it.
		const Int numIterations ) const;

private:
	const DisplayProjectorAligned* m_DisplayModel = nullptr;
	const ObserverSpace* m_ObserverSpace = nullptr;
	std::shared_ptr<DiffuserModel> m_DiffuserModel = nullptr;
};


#endif // DISPLAYPROJECTORSOPTIMIZATION_H