#include "RayGenPinhole.h"

using namespace lfrt;



RayGenPinhole::RayGenPinhole(
    const Int& width, const Int& height,
    const VEC2& imagePlaneHalfSize,
    const Real& imagePlaneDepth )
    : Width(width)
    , Height(height)
    , ImagePlaneHalfSize(imagePlaneHalfSize)
    , ImagePlaneDepth(imagePlaneDepth)
{
}



Real RayGenPinhole::GenerateRay(
    const VEC2& raster, const VEC2& secondary,
    VEC3& ori, VEC3& dir ) const
{
    if ( raster.x < 0 || raster.y < 0 || raster.x > Width || raster.y > Height )
        return 0.0;

    const Real lambdaX = -1.0 + 2.0 * raster.x / Real(Width);
    const Real lambdaY = -1.0 + 2.0 * raster.y / Real(Height);

    const Real posX =  lambdaX * ImagePlaneHalfSize.x;
    const Real posY = -lambdaY * ImagePlaneHalfSize.y;
    const Real posZ = ImagePlaneDepth;

    ori = { OriginXY.x, OriginXY.y, 0 };
    dir = { posX, posY, posZ };

    return 1.0;
}
