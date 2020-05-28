#pragma once

#include "BaseTypes.h"
#include "BaseEnums.h"

#include <vector>


namespace LFOptics
{


class ILFSystem3
{
public:
	virtual Vec4 Ray3ToLF( const Ray3& ray ) const = 0;
	virtual Ray3 LFToRay3( const Vec4& coord ) const = 0;
};


class ILFTransform
{
public:
	virtual Vec4 LFTransform( const Vec4& coord, const Real signDir = Real(1) ) const = 0;
};


class ILFProjection
{
public:
	virtual Vec4 LFProjection( const Vec3& point ) const = 0;
};


class IRayColorizer
{
public:
	virtual Vec4 ProcessRay( const Ray3& ray ) const = 0;
};


class IRayGenerator
{
public:
	virtual Ray3 GenerateRay( const Vec2& surfaceCoord ) const = 0;
};


class IRayGeneratorOnImageMulti
{
public:
	virtual Int NumSamples() const = 0;
	virtual void GenerateRays(std::vector<Vec4>& colorWeights, std::vector<Ray3>& rays, const Vec2i& imageCoord) const = 0;
};


} // namespace LFOptics