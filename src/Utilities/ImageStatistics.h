#ifndef UTILITIES_IMAGESTATISTICS_H
#define UTILITIES_IMAGESTATISTICS_H

#include "BaseTypes.h"



class ImageStatistics
{
public:
	struct Elem
	{
		cv::Scalar mse = cv::Scalar();
		cv::Scalar msssim = cv::Scalar();
	};

public:
	ImageStatistics() = default;
	ImageStatistics( const cv::Mat& imageA, const cv::Mat& imageB ) { Evaluate( imageA, imageB ); }
	ImageStatistics( const std::vector<cv::Mat>& imagesA, const std::vector<cv::Mat>& imagesB ) { Evaluate( imagesA, imagesB ); }

	bool Evaluate( const cv::Mat& imageA, const cv::Mat& imageB );
	bool Evaluate( const std::vector<cv::Mat>& imagesA, const std::vector<cv::Mat>& imagesB );

	Elem Average() const;

	bool SaveToFile( const std::string& dirpath, const std::string& prefix = std::string() );

public:
	std::vector<Elem> data;
};


#endif // UTILITIES_IMAGESTATISTICS_H