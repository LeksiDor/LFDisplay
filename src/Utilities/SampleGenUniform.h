#ifndef SAMPLEGENUNIFORM_H
#define SAMPLEGENUNIFORM_H

#include "LFRayTracer.h"


// Simple uniform non-random sampler.
class SampleGenUniform : public lfrt::SampleGenerator
{
public:
    using Int = lfrt::Int;
    using Real = lfrt::Real;
    using VEC2 = lfrt::VEC2;

    SampleGenUniform( const Int primaryRes = 1, const Int secondaryRes = 1 );

    virtual SampleGenerator* Clone() const override;

    virtual bool ResetPixel( const Int& x, const Int& y ) override;

    virtual Int NumSamplesInPixel() override;

    virtual bool CurrentSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time ) override;

    virtual bool MoveToNextSample() override;

    Int primaryRes = 1;
    Int secondaryRes = 1;
    Int currentSampleInd = 0;
    Int x;
    Int y;
};



#endif // SAMPLEGENUNIFORM_H