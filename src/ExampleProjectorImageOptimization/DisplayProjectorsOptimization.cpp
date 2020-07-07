#include "DisplayProjectorsOptimization.h"

#include "DiffuserModel.h"
#include "DisplayProjectorAligned.h"
#include "ObserverSpace.h"


DisplayProjectorsOptimization::DisplayProjectorsOptimization( const DisplayProjectorAligned* displayModel, const ObserverSpace* viewerSpace )
	:m_DisplayModel(displayModel)
	,m_ObserverSpace(viewerSpace)
{
	m_DiffuserModel.reset( DiffuserModel::Create(*displayModel) );
}


bool DisplayProjectorsOptimization::Iterate(
	const std::vector<cv::Mat>& groundtrue, // Ground-true images for each position in the observer space.
	const std::vector<cv::Mat>& zeroIteration, // Initial set of projector images.
	std::vector< std::vector<cv::Mat> >& iterations, // Array of set of projector images; don't initialize it.
	const Int numIterations ) const
{
	// +++++ Initialize basic parameters and make sanity check. +++++
	
	if ( m_DisplayModel == nullptr )
		return false;
	if ( m_ObserverSpace == nullptr )
		return false;
	if ( numIterations <= 0 )
		return false;

	const Int width = m_DisplayModel->ProjectorResolution[0];
	const Int height = m_DisplayModel->ProjectorResolution[1];
	const Real halfSizeX = m_DisplayModel->HalfPhysSize[0];
	const Real halfSizeY = m_DisplayModel->HalfPhysSize[1];
	const Real viewerDistance = m_DisplayModel->ViewerDistance;

	if ( width <= 0 || height <= 0 )
		return false;

	const Int numViewerPositions = m_ObserverSpace->NumPositions();
	std::vector<Vec3> projectorPositions;
	m_DisplayModel->FillProjectorsPositions( projectorPositions );
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

	const int RealType = cv::DataType<Real>::type;

	// Parallelize pixel-wise.
	cv::parallel_for_( cv::Range( 0, width*height ),
		[&](const cv::Range& range)
		{
			cv::Mat B = cv::Mat( numProjectors, numProjectors, RealType );
			std::vector<cv::Mat> betas(3);
			cv::Mat w = cv::Mat( numProjectors, 1, RealType );

			cv::Mat prev = cv::Mat( numProjectors, 1, RealType );
			cv::Mat cur = cv::Mat( numProjectors, 1, RealType );
			cv::Mat gradient = cv::Mat( numProjectors, 1, RealType );
			cv::Mat descent = cv::Mat( numProjectors, 1, RealType );

			for ( int pixelInd = range.start; pixelInd < range.end; ++pixelInd )
			{
				const Int x = pixelInd % width;
				const Int y = pixelInd / width;

				// Find ray-screen intersection.
				const Real lambdaX = (Real(x) + 0.5) / Real(width);
				const Real lambdaY = (Real(y) + 0.5) / Real(height);
				const Real x0 =  halfSizeX * (2.0*lambdaX - 1.0);
				const Real y0 = -halfSizeY * (2.0*lambdaY - 1.0);
				const Real z0 = viewerDistance;

				// Reset beta and B.
				B = cv::Mat::zeros( numProjectors, numProjectors, RealType );
				for ( Int i = 0; i < 3; betas[i++] = cv::Mat::zeros( numProjectors, 1, RealType ) );
				// Evaluate beta and B.
				for ( Int viewInd = 0; viewInd < numViewerPositions; ++viewInd )
				{
					const Vec3 viewPos = m_ObserverSpace->Position( viewInd );
					const Color gtColor = groundtrue[viewInd].at<Color>(y,x);
					// Evaluate weights.
					Real sumWeights = 0;
					w = cv::Mat( numProjectors, 1, RealType );
					for ( Int projInd = 0; projInd < numProjectors; ++projInd )
					{
						const Vec3 projPos = projectorPositions[projInd];
						const Real weight = m_DiffuserModel->RefractedIntensity( projPos, Vec3(x0,y0,z0), viewPos );
						if ( weight > 0.00001 )
						{
							sumWeights += weight;
							w.at<Real>(projInd,0) = weight;
						}
					}
					// Normalize weights.
					if ( sumWeights > 0.00001 )
					{
						for ( Int projInd = 0; projInd < numProjectors; ++projInd )
						{
							w.at<Real>(projInd,0) /= sumWeights;
						}
						sumWeights = 1.0;
					}
					// Add partial sum to B and beta.
					for ( Int i = 0; i < numProjectors; ++i )
					{
						const Real weight = w.at<Real>(i,0);
						for ( Int i = 0; i < 3; ++i )
							betas[i].at<Real>(i,0) += weight * gtColor[i];
						for ( Int j = 0; j < numProjectors; ++j )
						{
							const Real weight_j = w.at<Real>(j,0);
							B.at<Real>(i,j) += weight * weight_j;
						}
					}
				}
				// Normalize B and beta.
				B /= Real(numViewerPositions);
				for ( Int i = 0; i < 3; betas[i++] /= Real(numViewerPositions) );

				// Iterate.
				for ( Int iterInd = 0; iterInd < numIterations; ++iterInd )
				{
					const std::vector<cv::Mat>& prevIterImage = (iterInd == 0) ? zeroIteration : iterations[iterInd-1];
					std::vector<cv::Mat>& curIterImage = iterations[iterInd];
					for ( Int channelInd = 0; channelInd < 3; ++channelInd )
					{
						// Initialize variables.
						const cv::Mat& beta = betas[channelInd];
						//cv::Mat& beta = { beta0, beta1, beta2 }[channelInd];
						for ( Int projInd = 0; projInd < numProjectors; ++projInd )
							prev.at<Real>(projInd,0) = prevIterImage[projInd].at<Color>(y,x)[channelInd];
						// Calculate gradient: gradient = B*R - beta.
						gradient = B*prev - beta;
						// Calculate descent: descent[i] = gradient[i]/B[i,i].
						for ( Int i = 0; i < numProjectors; ++i )
						{
							Real prev_val = prev.at<Real>(i,0);
							Real descent_val = gradient.at<Real>(i,0) / B.at<Real>(i,i);
							if ( descent_val < 0 && prev_val >= 1 ) descent_val = 0;
							if ( descent_val > 0 && prev_val <= 0 ) descent_val = 0;
							descent.at<Real>(i,0) = descent_val;
						}
						// Calculate optimal step value: lambda = (descent.gradient)/(descent.B.descent).
						const Real lambda_nom = descent.dot( gradient );
						const Real lambda_denom = descent.dot( B * descent );
						const Real lambda = lambda_nom / lambda_denom;
						// Calculate new iteration value.
						cur = prev - lambda * descent;
						// Store result to the image.
						for ( Int i = 0; i < numProjectors; ++i )
							curIterImage[i].at<Color>(y,x)[channelInd] = cur.at<Real>(i,0);
					}
				}
			}
		} );


	return true;
}
