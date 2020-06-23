#include "SampleAccumCV.h"



using namespace lfrt;



SampleAccumCV::SampleAccumCV( const Int width, const Int height )
{
    SetSize( width, height );
}



bool SampleAccumCV::SetSize( const Int& width, const Int& height )
{
    if ( width <= 0 || height <= 0 )
        return false;
    weighted   = cv::Mat::zeros( height, width, CV_32FC3 );
    weights    = cv::Mat::zeros( height, width, CV_32FC1 );
    unweighted = cv::Mat::zeros( height, width, CV_32FC3 );
    return true;
}



bool SampleAccumCV::GetRenderBounds( Int& startX, Int& startY, Int& endX, Int& endY ) const
{
    startX = 0;
    startY = 0;
    endX = Width();
    endY = Height();
    return true;
}


bool SampleAccumCV::GetSamplingBounds( Int& startX, Int& startY, Int& endX, Int& endY ) const
{
    startX = 0;
    startY = 0;
    endX = Width();
    endY = Height();
    return true;
}


SampleTile* SampleAccumCV::CreateSampleTile(
    const Int& startX, const Int& startY,
    const Int&   endX, const Int&   endY )
{
    return new SampleTileCV( startX, startY, endX, endY );
}


bool SampleAccumCV::MergeSampleTile( SampleTile* tile )
{
    SampleTileCV* filmTile = dynamic_cast<SampleTileCV*>( tile );
    if ( filmTile == nullptr )
        return false;
    const Int startX = filmTile->startX;
    const Int startY = filmTile->startY;
    for ( Int localX = 0; localX < filmTile->width; ++localX )
    {
        for ( Int localY = 0; localY < filmTile->height; ++localY )
        {
            weighted.at<RGB>( startY+localY, startX+localX ) = filmTile->weighted.at<RGB>( localY, localX );
            weights.at<Gray>( startY+localY, startX+localX ) = filmTile->weights.at<Gray>( localY, localX );
            unweighted.at<RGB>( startY+localY, startX+localX ) = filmTile->unweighted.at<RGB>( localY, localX );
        }
    }
    return true;
}


bool SampleAccumCV::DestroySampleTile( SampleTile* tile )
{
    SampleTileCV* filmTile = dynamic_cast<SampleTileCV*>( tile );
    if ( filmTile == nullptr )
        return false;
    delete filmTile;
    return true;
}


bool SampleAccumCV::GetColor( const Int& x, const Int& y, Real& r, Real& g, Real& b ) const
{
    if ( x < 0 || y < 0 || x >= Width() || y >= Height() )
        return false;
    const RGB& sum = weighted.at<RGB>(y,x);
    const Gray& w = weights.at<Gray>(y,x);
    const RGB& flat = unweighted.at<RGB>(y,x);
    r = flat[2];
    g = flat[1];
    b = flat[0];
    if ( w > 0 )
    {
        r += sum[2] / w;
        g += sum[1] / w;
        b += sum[0] / w;
    }
    return true;
}


void SampleAccumCV::SaveToImage( cv::Mat& image ) const
{
    const int width = Width();
    const int height = Height();

    image = cv::Mat( height, width, CV_32FC3 );

    cv::parallel_for_( cv::Range( 0, width*height ),
        [&]( const cv::Range& range )
        {
            for ( int r = range.start; r < range.end; ++r )
            {
                const int x = r % width;
                const int y = r / width;
                const RGB& a = unweighted.at<RGB>(y,x);
                const RGB& b = weighted.at<RGB>(y,x);
                const Gray& c = weights.at<Gray>(y,x);
                image.at<RGB>(y,x) = a + b / c;
            }
        }
    );
}


SampleTileCV::SampleTileCV( const Int& startX, const Int& startY, const Int& endX, const Int& endY )
    :startX(startX)
    ,startY(startY)
    ,width(endX-startX)
    ,height(endY-startY)
{
    weighted   = cv::Mat::zeros( height, width, CV_32FC3 );
    weights    = cv::Mat::zeros( height, width, CV_32FC1 );
    unweighted = cv::Mat::zeros( height, width, CV_32FC3 );
}


bool SampleTileCV::AddSample(
    const VEC2& raster, const VEC2& secondary,
    const Real& sampleWeight, const Real& rayWeight,
    const Real& r, const Real& g, const Real& b,
    const bool isWeighted )
{
    const Int x = Int(raster.x) - startX;
    const Int y = Int(raster.y) - startY;
    if ( x < 0 || y < 0 || x >= width || y >= width )
        return false;
    if ( isWeighted )
    {
        const Real w = sampleWeight * rayWeight;
        RGB& rgb = weighted.at<RGB>(y,x);
        rgb += w * RGB(b,g,r);
        weights.at<Gray>(y,x) += w;
    }
    else
    {
        RGB& rgb = unweighted.at<RGB>(y,x);
        rgb += RGB(b,g,r);
    }
    return true;
}
