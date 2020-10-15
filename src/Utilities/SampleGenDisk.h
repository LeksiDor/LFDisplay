#ifndef UTILITIES_SAMPLEGENDISK_H
#define UTILITIES_SAMPLEGENDISK_H

#include "LFRayTracer.h"

#include <vector>


// Primary sample coordinates are in uniform grid order.
// Secondary sample coordinates are in uniform grid that is inside the incircle.
class SampleGenDisk : public lfrt::SampleGenerator
{
public:
    using Int = lfrt::Int;
    using Real = lfrt::Real;
    using VEC2 = lfrt::VEC2;

    SampleGenDisk( const Int primaryRes = 1, const Int secondaryRes = 1 );

    virtual SampleGenerator* Clone() const override;

    virtual bool ResetPixel( const Int& x, const Int& y ) override;

    virtual Int NumSamplesInPixel() override;

    virtual bool CurrentSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time ) override;

    virtual bool MoveToNextSample() override;

    bool SetPrimaryRes( const Int primaryRes );
    bool SetSecondaryRes( const Int secondaryRes );

private:
    Int primaryRes = 1;
    Int secondaryRes = 1;
    Int currentSampleInd = 0;
    Int x;
    Int y;

    std::vector<VEC2> apertureCoords;
};


#endif // UTILITIES_SAMPLEGENDISK_H