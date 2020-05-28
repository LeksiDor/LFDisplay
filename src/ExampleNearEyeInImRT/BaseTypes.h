#pragma once


#include "GeometryVector.h"
#include "GeometryMatrix.h"


namespace LFOptics
{

typedef double Real;
typedef int Int;

extern const Real RealNaN;
extern const Real RealMin;
extern const Real RealMax;

extern const Int IntNaN;
extern const Int IntMin;
extern const Int IntMax;

template<typename T> T TypeMin();
template<typename T> T TypeMax();
template<typename T> T TypeNaN();

template<> Real TypeMin<Real>();
template<> Int  TypeMin<Int>();
template<> Real TypeMax<Real>();
template<> Int  TypeMax<Int>();
template<> Real TypeNaN<Real>();
template<> Int  TypeNaN<Int>();


typedef vec<2,Real> Vec2;
typedef vec<3,Real> Vec3;
typedef vec<4,Real> Vec4;

typedef vec<2,Int> Vec2i;
typedef vec<3,Int> Vec3i;
typedef vec<4,Int> Vec4i;


typedef mat<2,2,Real> Mat22;
typedef mat<3,3,Real> Mat33;
typedef mat<4,4,Real> Mat44;

typedef affine2<Real> Affine2;
typedef affine3<Real> Affine3;


template<> Vec2 TypeNaN<Vec2>();
template<> Vec3 TypeNaN<Vec3>();
template<> Vec4 TypeNaN<Vec4>();

template<> Vec2i TypeNaN<Vec2i>();
template<> Vec3i TypeNaN<Vec3i>();
template<> Vec4i TypeNaN<Vec4i>();

template<> Mat22 TypeNaN<Mat22>();
template<> Mat33 TypeNaN<Mat33>();
template<> Mat44 TypeNaN<Mat44>();

template<> Affine2 TypeNaN<Affine2>();
template<> Affine3 TypeNaN<Affine3>();



struct Ray2
{
	union
	{
		struct { Vec2 origin; Vec2 direction; };
		Vec4 xyzw;
	};
	
public:
	Ray2() : origin(RealNaN,RealNaN), direction(RealNaN,RealNaN) { }
	Ray2( const Vec2 &ori, const Vec2 &dir ) : origin(ori), direction(dir) { }
};


struct Ray3
{
	Vec3 origin;
	Vec3 direction;
public:
	Ray3() : origin(RealNaN,RealNaN,RealNaN), direction(RealNaN,RealNaN,RealNaN) { }
	Ray3(const Vec3 &ori, const Vec3 &dir ) : origin(ori), direction(dir) { }
	Ray2 ToRay2() const { return Ray2( origin.xy/origin.z, direction.xy/direction.z ); }
	Ray2 ToRay2( const Real& refPlaneZ ) const
	{
		const Vec2 dir2 = direction.xy/direction.z;
		const Vec2 ori2 = origin.xy + (refPlaneZ - origin.z) * dir2;
		return Ray2( ori2, dir2 );
	}
};



template<typename T> struct MinMax2
{
	vec<2,T> min;
	vec<2,T> max;
	MinMax2() : min(TypeMax<T>(),TypeMax<T>()), max(TypeMin<T>(),TypeMin<T>()) { }
	MinMax2( const vec<2,T>& min, const vec<2,T>& max ) : min(min), max(max) { }
	MinMax2( const T& minX, const T& minY, const T& maxX, const T& maxY )
		: min(minX,minY), max(maxX,maxY) { }
	MinMax2<T> add( const vec<2,T>& point ) const;
	MinMax2<T> add( const MinMax2<T>& minmax ) const;
	MinMax2<T> intersect( const MinMax2& minmax ) const;
	MinMax2<T> translate( const vec<2,T>& point ) const;
	vec<2,T> center() const;
	vec<2,T> range() const;
	bool isInRange( const vec<2,T> point ) const;
	bool isValid() const;
};


typedef MinMax2<Int> MinMax2Int;
typedef MinMax2<Real> MinMax2Real;



Ray3 inline operator * ( const Affine3 &transform, const Ray3 &ray )
{
	return Ray3( transform.rotation*ray.origin + transform.translation, transform.rotation*ray.direction );
}



} // namespace LFOptics