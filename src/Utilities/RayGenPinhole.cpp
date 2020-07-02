#include "RayGenPinhole.h"

using namespace lfrt;



RayGenPinhole::RayGenPinhole( const Int& width, const Int& height )
    :Width(width), Height(height)
    ,MinX(-1.0), MinY(-1.0)
    ,MaxX(1.0), MaxY(1.0)
    ,ImagePlaneDepth(1.0)
    ,OriginX(0.0), OriginY(0.0)
{
}


RayGenPinhole::RayGenPinhole(
    const Int& width, const Int& height,
    const Real& imagePlaneHalfSizeX,
    const Real& imagePlaneHalfSizeY,
    const Real& imagePlaneDepth )
    :Width(width), Height(height)
    ,MinX(-imagePlaneHalfSizeX), MinY(-imagePlaneHalfSizeY)
    ,MaxX(imagePlaneHalfSizeX), MaxY(imagePlaneHalfSizeY)
    ,ImagePlaneDepth(imagePlaneDepth)
    ,OriginX(0.0), OriginY(0.0)
{
}


RayGenPinhole::RayGenPinhole(
    const Int& width, const Int& height,
    const Real& minX, const Real& minY,
    const Real& maxX, const Real& maxY,
    const Real& imagePlaneDepth )
    :Width(width), Height(height)
    ,MinX(minX), MinY(minY)
    ,MaxX(maxX), MaxY(maxY)
    ,ImagePlaneDepth(imagePlaneDepth)
    ,OriginX(0.0), OriginY(0.0)
{
}


RayGenPinhole::RayGenPinhole(
    const Int& width, const Int& height,
    const Real& minX, const Real& minY,
    const Real& maxX, const Real& maxY,
    const Real& imagePlaneDepth,
    const Real& originX, const Real& originY )
    :Width(width), Height(height)
    ,MinX(minX), MinY(minY)
    ,MaxX(maxX), MaxY(maxY)
    ,ImagePlaneDepth(imagePlaneDepth)
    ,OriginX(originX), OriginY(originY)
{
}


Real RayGenPinhole::GenerateRay(
    const VEC2& raster, const VEC2& secondary,
    VEC3& ori, VEC3& dir ) const
{
    if ( raster.x < 0 || raster.y < 0 || raster.x > Width || raster.y > Height )
        return 0.0;

    const Real lambdaX = raster.x / Real(Width);
    const Real lambdaY = 1.0 - raster.y / Real(Height);

    const Real posX = MinX + lambdaX * (MaxX - MinX);
    const Real posY = MinY + lambdaY * (MaxY - MinY);
    const Real posZ = ImagePlaneDepth;

    ori = { OriginX, OriginY, 0 };
    dir = { posX - OriginX, posY - OriginY, posZ };

    return 1.0;
}
