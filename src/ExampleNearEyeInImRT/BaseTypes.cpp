#include "BaseTypes.h"

#include <algorithm>
#include <limits>



namespace LFOptics
{

const Real RealNaN = std::numeric_limits<Real>::has_quiet_NaN ? std::numeric_limits<Real>::quiet_NaN() : std::numeric_limits<Real>::max();
const Real RealMin = std::numeric_limits<Real>::min();
const Real RealMax = std::numeric_limits<Real>::max();

const Int IntNaN = std::numeric_limits<Int>::has_quiet_NaN ? std::numeric_limits<Int>::quiet_NaN() : std::numeric_limits<Int>::max();
const Int IntMin = std::numeric_limits<Int>::min();
const Int IntMax = std::numeric_limits<Int>::max();



template<> Real TypeMin<Real>() { return RealMin; }
template<> Int  TypeMin<Int>() { return IntMin; }
template<> Real TypeMax<Real>() { return RealMax; }
template<> Int  TypeMax<Int>() { return IntMax; }
template<> Real TypeNaN<Real>() { return RealNaN; }
template<> Int  TypeNaN<Int>() { return IntNaN; }

template<> Vec2 TypeNaN<Vec2>() { return Vec2( RealNaN, RealNaN ); }
template<> Vec3 TypeNaN<Vec3>() { return Vec3( RealNaN, RealNaN, RealNaN ); }
template<> Vec4 TypeNaN<Vec4>() { return Vec4( RealNaN, RealNaN, RealNaN, RealNaN ); }

template<> Vec2i TypeNaN<Vec2i>() { return Vec2i( TypeNaN<Int>(), TypeNaN<Int>() ); }
template<> Vec3i TypeNaN<Vec3i>() { return Vec3i( TypeNaN<Int>(), TypeNaN<Int>(), TypeNaN<Int>() ); }
template<> Vec4i TypeNaN<Vec4i>() { return Vec4i( TypeNaN<Int>(), TypeNaN<Int>(), TypeNaN<Int>(), TypeNaN<Int>() ); }

template<> Mat22 TypeNaN<Mat22>() { return Mat22( RealNaN, RealNaN, RealNaN, RealNaN ); }
template<> Mat33 TypeNaN<Mat33>() { return Mat33( RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN ); }
template<> Mat44 TypeNaN<Mat44>() { return Mat44( RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN, RealNaN ); }

template<> Affine2 TypeNaN<Affine2>() { return Affine2( TypeNaN<Mat22>(), TypeNaN<Vec2>() ); }
template<> Affine3 TypeNaN<Affine3>() { return Affine3( TypeNaN<Mat33>(), TypeNaN<Vec3>() ); }



template<typename T>
MinMax2<T> MinMax2<T>::add( const vec<2,T>& point ) const
{
	return MinMax2(
		vec<2,T>( std::min<T>(min.x,point.x), std::min<T>(min.y,point.y) ),
		vec<2,T>( std::max<T>(max.x,point.x), std::max<T>(max.y,point.y) )
	);
}


template<typename T>
MinMax2<T> MinMax2<T>::add( const MinMax2<T>& minmax ) const
{
	return MinMax2(
		vec<2,T>( std::min<T>(min.x,minmax.min.x), std::min<T>(min.y,minmax.min.y) ),
		vec<2,T>( std::max<T>(max.x,minmax.max.x), std::max<T>(max.y,minmax.max.y) )
	);
}


template<typename T> 
MinMax2<T> MinMax2<T>::intersect( const MinMax2& minmax ) const
{
	return MinMax2(
		vec<2,T>( std::max<T>(min.x,minmax.min.x), std::max<T>(min.y,minmax.min.y) ),
		vec<2,T>( std::min<T>(max.x,minmax.max.x), std::min<T>(max.y,minmax.max.y) )
	);
}


template<typename T>
MinMax2<T> MinMax2<T>::translate( const vec<2,T>& point ) const
{
	return MinMax2( point + min, point + max );
}


template<typename T>
vec<2,T> MinMax2<T>::center() const
{
	return vec<2,T>( max.x + min.x, max.y + min.y ) / 2;
}


template<typename T>
vec<2,T> MinMax2<T>::range() const
{
	return vec<2,T>( max.x - min.x, max.y - min.y );
}


template<typename T>
bool MinMax2<T>::isInRange( const vec<2,T> point ) const
{
	return (point.x >= min.x) && (point.x <= max.x)
		&& (point.y >= min.y) && (point.y <= max.y);
}


template<typename T>
bool MinMax2<T>::isValid() const
{
	return (min.x <= max.x) && (min.y <= max.y)
		&& (min.x != TypeNaN<T>()) && (max.x != TypeNaN<T>())
		&& (min.y != TypeNaN<T>()) && (max.y != TypeNaN<T>());
}





void __implementation_BaseTypes__()
{
	{
		MinMax2Int dummy;
		dummy.add( dummy.min );
		dummy.add( dummy );
		dummy.intersect( dummy );
		dummy.translate( dummy.min );
		dummy.center();
		dummy.range();
		dummy.isValid();
		dummy.isInRange( dummy.min );
	}
	{
		MinMax2Real dummy;
		dummy.add( dummy.min );
		dummy.add( dummy );
		dummy.intersect( dummy );
		dummy.translate( dummy.min );
		dummy.center();
		dummy.range();
		dummy.isValid();
		dummy.isInRange( dummy.min );
	}
}


} // namespace LFOptics