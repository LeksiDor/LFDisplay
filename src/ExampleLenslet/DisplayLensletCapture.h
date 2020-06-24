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

public:

    DisplayLensletCapture( const DisplayLenslet* model = nullptr );

    virtual ~DisplayLensletCapture() = default;


    // Inherited via RayGenerator
    virtual Real GenerateRay(
        const VEC2& raster, const VEC2& secondary,
        VEC3& ori, VEC3& dir ) const override;


public:
    const DisplayLenslet* DisplayModel = nullptr;
};


#endif // DISPLAYLENSLETCAPTURE_H