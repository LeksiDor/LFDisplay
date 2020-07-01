#ifndef UTILITIES_IMAGEANALYSIS_H
#define UTILITIES_IMAGEANALYSIS_H

#include <opencv2/core.hpp>


double ImagePSNR( const cv::Mat& I1, const cv::Mat& I2);

cv::Scalar ImageMSSIM( const cv::Mat& I1, const cv::Mat& I2);



#endif // UTILITIES_IMAGEANALYSIS_H