#pragma once

#define _USE_MATH_DEFINES
#include "BaseTypes.h"
#include <vector>

#include "BaseInterfaces.h"
#include "RendererBaseTypes.h"


// Based on tinyraytracer project: https://github.com/ssloy/tinyraytracer


namespace LFOptics
{

class Image2D;
class Image3D;


class RayTracer : public IRayColorizer
{
public:
	RayTracer(Scene* scene = nullptr);
	~RayTracer();

	Scene*& GetScene() { return m_Scene; }
	const Scene* GetScene() const { return m_Scene; }

	bool& IsFinalColorOpaque() { return m_IsFinalColorOpaque; }
	const bool& IsFinalColorOpaque() const { return m_IsFinalColorOpaque; }

	bool SetMaxRayDepth( const Int raydepth );

	// Inherited via IRayColorizer
	virtual Vec4 ProcessRay(const Ray3& ray) const override;

	// Ray direction must be normalized here.
	Vec4 CastRay( const Ray3& ray, const Int depth = 0 ) const;



private:
	Vec3 Reflect( const Vec3 &I, const Vec3 &N ) const;
	Vec3 Refract( const Vec3 &I, const Vec3 &N, const Real eta_t, const Real eta_i = 1.0f ) const;

private:
	Scene* m_Scene = nullptr;
	Int maxRayDepth;
	bool m_IsFinalColorOpaque = true;
};


} // namespace LFOptics