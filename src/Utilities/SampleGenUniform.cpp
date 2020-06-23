#include "SampleGenUniform.h"


using namespace lfrt;


SampleGenUniform::SampleGenUniform( const Int primaryRes, const Int secondaryRes )
    :primaryRes(primaryRes)
    ,secondaryRes(secondaryRes)
{
}


SampleGenerator* SampleGenUniform::Clone() const
{
    return new SampleGenUniform( primaryRes );
}


bool SampleGenUniform::ResetPixel( const Int& x, const Int& y )
{
    this->x = x;
    this->y = y;
    currentSampleInd = 0;
    return true;
}


Int SampleGenUniform::NumSamplesInPixel()
{
    return primaryRes * primaryRes * secondaryRes * secondaryRes;
}


bool SampleGenUniform::CurrentSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time )
{
    Int index = currentSampleInd;
    const Int indY2 = index % secondaryRes;
    const Int indX2 = (index /= secondaryRes) % secondaryRes;
    const Int indY1 = (index /= secondaryRes) % primaryRes;
    const Int indX1 = (index /= primaryRes) % primaryRes;
    weight = 1.0;
    const Real dx1 = (0.5+Real(indX1)) / Real(primaryRes);
    const Real dy1 = (0.5+Real(indY1)) / Real(primaryRes);
    const Real dx2 = (0.5+Real(indX2)) / Real(secondaryRes);
    const Real dy2 = (0.5+Real(indY2)) / Real(secondaryRes);
    raster = { Real(x)+dx1, Real(y)+dy1 };
    secondary = { dx2, dy2 };
    time = 0.0;
    return true;
}


bool SampleGenUniform::MoveToNextSample()
{
    ++currentSampleInd;
    return currentSampleInd < primaryRes*primaryRes*secondaryRes*secondaryRes;
}