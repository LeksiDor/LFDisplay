#include "DisplayProjectorIterations.h"

#include "DisplayProjectorAligned.h"
#include "ObserverSpace.h"


DisplayProjectorIterations::DisplayProjectorIterations( const DisplayProjectorAligned* displayModel, const ObserverSpace* viewerSpace )
	:DisplayModel(displayModel)
	,ViewerSpace(viewerSpace)
{
}


bool DisplayProjectorIterations::Iterate(
	const std::vector<cv::Mat>& groundtrue, // Ground-true images for each position in the observer space.
	const std::vector<cv::Mat>& zeroIteration, // Initial set of projector images.
	std::vector< std::vector<cv::Mat> >& iterations, // Array of set of projector images; don't initialize it.
	const Int numIterations ) const
{
	// +++++ Initialize basic parameters and make sanity check. +++++
	
	if ( DisplayModel == nullptr )
		return false;
	if ( ViewerSpace == nullptr )
		return false;
	if ( numIterations <= 0 )
		return false;

	const Int width = DisplayModel->ProjectorResolution[0];
	const Int height = DisplayModel->ProjectorResolution[1];

	if ( width <= 0 || height <= 0 )
		return false;

	const Int numViewerPositions = ViewerSpace->NumPositions();
	std::vector<Vec3> projectorPositions;
	DisplayModel->FillProjectorsPositions( projectorPositions );
	const Int numProjectors = projectorPositions.size();

	if ( numProjectors <= 0 )
		return false;
	if ( groundtrue.size() != numViewerPositions )
		return false;
	if ( zeroIteration.size() != numProjectors )
		return false;
	for ( Int i = 0; i < numViewerPositions; ++i )
	{
		const cv::Mat& image = groundtrue[i];
		if ( image.cols != width || image.rows != height || image.type() != CV_32FC3 )
			return false;
	}
	for ( Int i = 0; i < numProjectors; ++i )
	{
		const cv::Mat& image = zeroIteration[i];
		if ( image.cols != width || image.rows != height || image.type() != CV_32FC3 )
			return false;
	}

	iterations.resize( numIterations );
	for ( Int iterInd = 0; iterInd < numIterations; ++iterInd )
	{
		std::vector<cv::Mat>& iter = iterations[iterInd];
		iter.resize( numProjectors );
		for ( Int projInd = 0; projInd < numProjectors; ++projInd )
			iter[projInd] = cv::Mat::zeros( height, width, CV_32FC3 );
	}
	// ----- Initialize basic parameters and make sanity check. -----


	// Parallelize pixel-wise.
	cv::parallel_for_( cv::Range( 0, width*height ),
		[&](const cv::Range& range)
		{
			cv::Mat B = cv::Mat( numProjectors, numProjectors, cv::DataType<Real>::type );
			cv::Mat betaR = cv::Mat( numProjectors, 1, cv::DataType<Real>::type );
			cv::Mat betaG = cv::Mat( numProjectors, 1, cv::DataType<Real>::type );
			cv::Mat betaB = cv::Mat( numProjectors, 1, cv::DataType<Real>::type );
			cv::Mat w = cv::Mat( numProjectors, 1, cv::DataType<Real>::type );

			for ( int pixelInd = range.start; pixelInd < range.end; ++pixelInd )
			{
				const Int x = pixelInd % width;
				const Int y = pixelInd / width;

				{
					// Initialize beta and B.
					for ( Int i = 0; i < numProjectors; ++i )
					{
						//betaR.at<Real>(i,0) = 
					}
				}
			}
		} );


	return false;
}
