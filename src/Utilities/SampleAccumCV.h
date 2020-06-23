#ifndef SAMPLEACCUMCV_H
#define SAMPLEACCUMCV_H

#include "LFRayTracer.h"
#include <opencv2/opencv.hpp>




class SampleAccumCV : public lfrt::SampleAccumulator
{
public:
    using Int = lfrt::Int;
    using Real = lfrt::Real;
    using RGB = cv::Vec3f;
    using Gray = float;

public:
    SampleAccumCV( const Int width, const Int height );

    // Inherited via SampleAccumulator
    virtual bool SetSize(const Int& width, const Int& height) override;
    virtual Int Width() const override { return weighted.cols;}
    virtual Int Height() const override { return weighted.rows; }
    virtual bool GetRenderBounds(
        Int& startX, Int& startY, Int& endX, Int& endY) const override;
    virtual bool GetSamplingBounds(
        Int& startX, Int& startY, Int& endX, Int& endY) const override;
    virtual lfrt::SampleTile* CreateSampleTile(
        const Int& startX, const Int& startY,
        const Int&   endX, const Int&   endY ) override;
    virtual bool MergeSampleTile( lfrt::SampleTile* tile ) override;
    virtual bool DestroySampleTile( lfrt::SampleTile* tile ) override;
    virtual bool GetColor( const Int& x, const Int& y, Real& r, Real& g, Real& b ) const override;


    const cv::Mat& Weighted() const { return weighted; }
    const cv::Mat& Weights() const { return weights; }
    const cv::Mat& Unweighted() const { return unweighted; }

    void SaveToImage( cv::Mat& image ) const;

private:
    cv::Mat weighted;
    cv::Mat weights;
    cv::Mat unweighted;
};



class SampleTileCV : public lfrt::SampleTile
{
public:
    friend class SampleAccumCV;
    using VEC2 = lfrt::VEC2;
    using Real = lfrt::Real;
    using Int = lfrt::Int;
    using RGB = cv::Vec3f;
    using Gray = float;

protected:
    virtual ~SampleTileCV() = default;

public:

    SampleTileCV(
        const Int& startX, const Int& startY,
        const Int&   endX, const Int&   endY );

    virtual bool AddSample(const VEC2& raster, const VEC2& secondary,
        const Real& sampleWeight, const Real& rayWeight,
        const Real& r, const Real& g, const Real& b,
        const bool isWeighted = true) override;

private:
    cv::Mat weighted;
    cv::Mat weights;
    cv::Mat unweighted;
    Int startX = 0;
    Int startY = 0;
    Int width = 0;
    Int height = 0;
};



#endif // SAMPLEACCUMCV_H