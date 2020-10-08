#include "ImageStatistics.h"

#include "ImageAnalysis.h"

#include <fstream>



bool ImageStatistics::Evaluate( const cv::Mat& imageA, const cv::Mat& imageB )
{
    return Evaluate( { imageA }, { imageB } );
}


bool ImageStatistics::Evaluate( const std::vector<cv::Mat>& imagesA, const std::vector<cv::Mat>& imagesB )
{
	const Int numImages = imagesA.size();
	if ( numImages <= 0 || imagesB.size() != numImages )
		return false;

    const Int width  = imagesA[0].cols;
    const Int height = imagesA[0].rows;
    for ( Int imageInd = 0; imageInd < numImages; ++imageInd )
    {
        if ( imagesA[0].cols != width || imagesA[0].rows != height )
            return false;
        if ( imagesB[0].cols != width || imagesB[0].rows != height )
            return false;
    }

    data.clear();
    data.resize( numImages );

    for ( Int imageInd = 0; imageInd < numImages; ++imageInd )
    {
        data[imageInd].mse = ImageValueMSE( imagesA[imageInd], imagesB[imageInd] );
        data[imageInd].msssim = ImageValueMSSIM( imagesA[imageInd], imagesB[imageInd] );
    }

    return true;
}


ImageStatistics::Elem ImageStatistics::Average() const
{
    const Int numImages = data.size();
    if ( numImages <= 0 )
        throw std::logic_error( "Error: Statistics array size is zero." );
    ImageStatistics::Elem sum;
    for ( Int imageInd = 0; imageInd < numImages; ++imageInd )
    {
        sum.mse += data[imageInd].mse;
        sum.msssim += data[imageInd].msssim;
    }
    sum.mse *= 1.0 / Real(numImages);
    sum.msssim *= 1.0 / Real(numImages);
    return sum;
}


bool ImageStatistics::SaveToFile( const std::string& dirpath, const std::string& prefix )
{
    const std::string mse_filepath    = dirpath + "/" + prefix + "mse.txt";
    const std::string msssim_filepath = dirpath + "/" + prefix + "msssim.txt";

    std::fstream file_mse( mse_filepath, std::fstream::out );
    std::fstream file_msssim( msssim_filepath, std::fstream::out );

    if ( !file_mse.is_open() || !file_msssim.is_open() )
    {
        file_mse.close();
        file_msssim.close();
        return false;
    }

    const Int numImages = data.size();

    for ( Int imageInd = 0; imageInd < numImages; ++imageInd )
    {
        const cv::Scalar mse_val = data[imageInd].mse;
        const cv::Scalar msssim_val = data[imageInd].msssim;

        file_mse << mse_val[0] << " " << mse_val[1] << " " << mse_val[2] << std::endl;
        file_msssim << msssim_val[0] << " " << msssim_val[1] << " " << msssim_val[2] << std::endl;
    }

    file_mse.close();
    file_msssim.close();

    return true;
}