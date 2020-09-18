#ifndef DISPLAYPROJECTORSCAPTURE_H
#define DISPLAYPROJECTORSCAPTURE_H

#include "BaseTypes.h"
//#include "DisplayProjectorAligned.h"
#include "LFRayTracer.h"

class DiffuserModel;
class DisplayProjectorAligned;

class DisplayProjectorsCapture
    : public lfrt::RayGenerator
{
public:
    using VEC2 = lfrt::VEC2;
    using VEC3 = lfrt::VEC3;

public:

    DisplayProjectorsCapture( const DisplayProjectorAligned* model, const Vec3& projectorPos );

    virtual ~DisplayProjectorsCapture() = default;


    // Inherited via RayGenerator
    virtual Real GenerateRay(
        const VEC2& raster, const VEC2& secondary,
        VEC3& ori, VEC3& dir ) const override;


private:
    const DisplayProjectorAligned* m_DisplayModel = nullptr;
    Vec3 m_ProjectorPosition = Vec3(0,0,0);
    std::shared_ptr<DiffuserModel> m_DiffuserModel = nullptr;
};


#endif // DISPLAYPROJECTORSCAPTURE_H