#ifndef UTILITIES_IMAGE_H
#define UTILITIES_IMAGE_H

#include <string>

namespace cv { class Mat; }


// Loads image and converts it into OpenCV 32FC1 format.
bool LoadImageGray( const std::string& filepath, cv::Mat& image );

// Loads image and converts it into OpenCV 32FC3 format.
bool LoadImageRGB( const std::string& filepath, cv::Mat& image );


#endif // UTILITIES_IMAGE_H