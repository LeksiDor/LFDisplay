#ifndef DISPLAYPROJECTORSCAPTURE_H
#define DISPLAYPROJECTORSCAPTURE_H

#include "BaseTypes.h"
#include "DisplayProjectorAligned.h"
#include "LFRayTracer.h"


class DisplayProjectorsCapture
    : public lfrt::RayGenerator
{
public:
    using VEC2 = lfrt::VEC2;
    using VEC3 = lfrt::VEC3;

public:

    DisplayProjectorsCapture( const DisplayProjectorAligned* model = nullptr );

    virtual ~DisplayProjectorsCapture() = default;


    // Inherited via RayGenerator
    virtual Real GenerateRay(
        const VEC2& raster, const VEC2& secondary,
        VEC3& ori, VEC3& dir ) const override;


public:
    const DisplayProjectorAligned* DisplayModel = nullptr;
    Vec3 ProjectorPosition = Vec3(0,0,0);
};


#endif // DISPLAYPROJECTORSCAPTURE_H