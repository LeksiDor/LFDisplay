#include "ImageAnalysis.h"
#include "Parallel.h"

#include <opencv2/opencv.hpp>

#include <limits>

const double Infinity = std::numeric_limits<double>::infinity();



cv::Scalar ImageValueMSE( const cv::Mat& imageA, const cv::Mat& imageB )
{
    cv::Mat imageDiff;
    absdiff( imageA, imageB, imageDiff );
    imageDiff = imageDiff.mul( imageDiff );
    const cv::Scalar sse = cv::sum( imageDiff );
    return sse / Real( imageA.total() );
}



cv::Scalar ImageValuePSNR( const cv::Mat& imageA, const cv::Mat& imageB )
{
    return MSE_to_PSNR( ImageValueMSE( imageA, imageB ) );
}



cv::Scalar MSE_to_PSNR( const cv::Scalar& mse )
{
    return cv::Scalar(
        mse[0] <= 0 ? Infinity : -10.0 * std::log10( mse[0] ),
        mse[1] <= 0 ? Infinity : -10.0 * std::log10( mse[1] ),
        mse[2] <= 0 ? Infinity : -10.0 * std::log10( mse[2] ),
        mse[3] <= 0 ? Infinity : -10.0 * std::log10( mse[3] )
    );
}



cv::Scalar ImageValueMSSIM( const cv::Mat& i1, const cv::Mat& i2)
{
    // Source: https://docs.opencv.org/3.4/d5/dc4/tutorial_video_input_psnr_ssim.html
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d = CV_32F;
    cv::Mat I1, I2;
    i1.convertTo(I1, d);            // cannot calculate on one byte large values
    i2.convertTo(I2, d);
    cv::Mat I2_2   = I2.mul(I2);        // I2^2
    cv::Mat I1_2   = I1.mul(I1);        // I1^2
    cv::Mat I1_I2  = I1.mul(I2);        // I1 * I2
                                    /*************************** END INITS **********************************/
    cv::Mat mu1, mu2;                   // PRELIMINARY COMPUTING
    cv::GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
    cv::GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
    cv::Mat mu1_2   =   mu1.mul(mu1);
    cv::Mat mu2_2   =   mu2.mul(mu2);
    cv::Mat mu1_mu2 =   mu1.mul(mu2);
    cv::Mat sigma1_2, sigma2_2, sigma12;
    cv::GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;
    cv::GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;
    cv::GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;
    cv::Mat t1, t2, t3;
    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);                 // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);                 // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
    cv::Mat ssim_map;
    cv::divide(t3, t1, ssim_map);        // ssim_map =  t3./t1;
    cv::Scalar mssim = cv::mean(ssim_map);   // mssim = average of ssim map
    return mssim;
}



bool ClampImages( std::vector<cv::Mat>& images )
{
    const Int numImages = images.size();
    if ( numImages <= 0 )
        return false;
    const Int width = images[0].cols;
    const Int height = images[0].rows;
    if ( width <= 0 || height <= 0 )
        return false;
    for ( Int imageInd = 0; imageInd < numImages; ++imageInd )
    {
        if ( images[0].cols != width || images[0].rows != height )
            return false;
    } 
    CVParallel2D( width, height, [&](Int x, Int y)
        {
            for ( Int imageInd = 0; imageInd < numImages; ++imageInd )
            {
                cv::Vec3f& val = images[imageInd].at<cv::Vec3f>(y,x);
                val = cv::Vec3f(
                    std::min<float>( std::max<float>( val(0), 0 ), 1 ),
                    std::min<float>( std::max<float>( val(1), 0 ), 1 ),
                    std::min<float>( std::max<float>( val(2), 0 ), 1 )
                );
            }
        }
    );
    return true;
}