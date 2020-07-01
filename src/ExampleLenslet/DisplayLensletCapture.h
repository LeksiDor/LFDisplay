#ifndef DISPLAYLENSLETCAPTURE_H
#define DISPLAYLENSLETCAPTURE_H

#include "BaseTypes.h"
#include "DisplayLenslet.h"
#include "LFRayTracer.h"


class DisplayLensletCapture
    : public lfrt::RayGenerator
{
public:
    using VEC2 = lfrt::VEC2;
    using VEC3 = lfrt::VEC3;

    enum class Sampling
    {
        LensletCenter = 0,
        PupilCenter = 1,
        LensletAverage = 2,
    };

public:

    DisplayLensletCapture(
        const DisplayLenslet* model = nullptr,
        const Sampling& sampling = Sampling::LensletCenter );

    virtual ~DisplayLensletCapture() = default;


    // Inherited via RayGenerator
    virtual Real GenerateRay(
        const VEC2& raster, const VEC2& secondary,
        VEC3& ori, VEC3& dir ) const override;


public:
    const DisplayLenslet* DisplayModel = nullptr;
    Sampling SamplingType = Sampling::LensletCenter;
};


#endif // DISPLAYLENSLETCAPTURE_H