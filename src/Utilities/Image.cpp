#include "Image.h"

#include <opencv2/opencv.hpp>


bool LoadImageGray( const std::string& filepath, cv::Mat& image )
{
	cv::Mat imageOriginal = cv::imread( filepath, cv::ImreadModes::IMREAD_ANYCOLOR | cv::ImreadModes::IMREAD_ANYDEPTH );
	if ( !imageOriginal.data )
		return false;
	double scaling = 1.0;
	const int depthSrc = imageOriginal.depth();
	if ( depthSrc == CV_8U  ) scaling = 1.0 / 255.0;
	if ( depthSrc == CV_16U ) scaling = 1.0 / 65535.0;
	imageOriginal.convertTo( image, CV_MAKETYPE(CV_32F,1), scaling );
	return true;
}


bool LoadImageRGB( const std::string& filepath, cv::Mat& image )
{
	cv::Mat imageOriginal = cv::imread( filepath, cv::ImreadModes::IMREAD_ANYCOLOR | cv::ImreadModes::IMREAD_ANYDEPTH );
	if ( !imageOriginal.data )
		return false;
	double scaling = 1.0;
	const int depthSrc = imageOriginal.depth();
	if ( depthSrc == CV_8U  ) scaling = 1.0 / 255.0;
	if ( depthSrc == CV_16U ) scaling = 1.0 / 65535.0;
	imageOriginal.convertTo( image, CV_MAKETYPE(CV_32F,3), scaling );
	return true;
}
