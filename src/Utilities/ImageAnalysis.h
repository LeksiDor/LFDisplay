#ifndef UTILITIES_IMAGEANALYSIS_H
#define UTILITIES_IMAGEANALYSIS_H

#include "BaseTypes.h"


cv::Scalar ImageValueMSE( const cv::Mat& imageA, const cv::Mat& imageB );

cv::Scalar ImageValuePSNR( const cv::Mat& imageA, const cv::Mat& imageB );

cv::Scalar MSE_to_PSNR( const cv::Scalar& mse );


cv::Scalar ImageValueMSSIM( const cv::Mat& I1, const cv::Mat& I2);



#endif // UTILITIES_IMAGEANALYSIS_H