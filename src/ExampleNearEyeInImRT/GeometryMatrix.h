#pragma once

#include "GeometryVector.h"


namespace LFOptics
{

template <size_t ROWS, size_t COLS, typename T> struct mat;
template <typename T> struct affine3;


template <size_t ROWS, size_t COLS, typename T> struct mat
{
	mat() { for ( size_t i = ROWS; i--; ) { for ( size_t j = COLS; j--; data_[i][j] = T() ); } }
	mat<ROWS, COLS, T> &operator = ( const mat<ROWS, COLS, T>& m ) { for ( size_t i = 0; i < ROWS; ++i ) { for ( size_t j = COLS; j--; data_[i][j] = m.data_[i][j] ); } return *this; }
	T& operator()(const size_t i, const size_t j)       { assert(i<ROWS); assert(j<COLS); return data_[i][j]; }
    const T& operator()(const size_t i, const size_t j) const { assert(i<ROWS); assert(j<COLS); return data_[i][j]; }
	static mat<ROWS,COLS,T> identity() { mat<ROWS,COLS,T> res; for ( size_t i = ROWS; i--; res(i,i) = (T)1 ); return res; }
	static mat<ROWS,COLS,T> zero() { return mat<ROWS,COLS,T>(); }
	vec<COLS,T> row(const size_t i) const { vec<COLS,T> v; for( size_t j = COLS; j--; v[j] = data_[i][j] ); return v; }
	vec<ROWS,T> column(const size_t j) const { vec<ROWS,T> v; for( size_t i = ROWS; i--; v[i] = data_[i][j] ); return v; }
private:
    T data_[ROWS][COLS];
};


template <typename T> struct mat<2,2,T>
{
	mat() { (*this)(0,0) = (*this)(0,1) = (*this)(1,0) = (*this)(1,1) = (T)0; }
	mat( const T& m00, const T& m01, const T& m10, const T& m11 )
		{ (*this)(0,0) = m00; (*this)(0,1) = m01; (*this)(1,0) = m10; (*this)(1,1) = m11; }
	mat( const vec<2,T>& col0, const vec<2,T>& col1 ) : mat(col0.x,col1.x,col0.y,col1.y) {}
	mat<2,2,T> &operator = ( const mat<2,2,T> &m ) { for ( size_t i = 0; i < 2; ++i ) { for ( size_t j = 2; j--; data_[i][j] = m.data_[i][j] ); } return *this; }
	T& operator()(const size_t i, const size_t j)       { assert(i<2); assert(j<2); return data_[i][j]; }
	const T& operator()(const size_t i, const size_t j) const { assert(i<2); assert(j<2); return data_[i][j]; }
	static mat<2,2,T> identity() { mat<2,2,T> res; for ( size_t i = 2; i--; res(i,i) = (T)1 ); return res; }
	static mat<2,2,T> zero() { return mat<2,2,T>(); }
	static mat<2,2,T> rotation(const T& angle);
	static mat<2,2,T> diagonalize( const vec<2,T>& diagonal ) { return mat<2,2,T>( diagonal.x, 0, 0, diagonal.y ); }
	static mat<2,2,T> diagonalize( const T& x, const T& y ) { return mat<2,2,T>( x, 0, 0, y ); }
	vec<2,T> row(const size_t i) const { vec<2,T> v; for( size_t j = 2; j--; v[j] = data_[i][j] ); return v; }
	vec<2,T> column(const size_t j) const { vec<2,T> v; for( size_t i = 2; i--; v[i] = data_[i][j] ); return v; }
	mat<2,2,T> inverse() const;
private:
	T data_[2][2];
};


template <typename T> struct mat<3,3,T>
{
	mat() { for ( size_t i = 3; i--; ) { for ( size_t j = 3; j--; data_[i][j] = T() ); } }
	mat( const T& m00, const T& m01, const T& m02,
		 const T& m10, const T& m11, const T& m12,
		 const T& m20, const T& m21, const T& m22 )
		{ (*this)(0,0) = m00; (*this)(0,1) = m01; (*this)(0,2) = m02;
		  (*this)(1,0) = m10; (*this)(1,1) = m11; (*this)(1,2) = m12;
		  (*this)(2,0) = m20; (*this)(2,1) = m21; (*this)(2,2) = m22; }
	mat( const vec<3,T>& col0, const vec<3,T>& col1, const vec<3,T>& col2 ) : mat(col0.x,col1.x,col2.x,col0.y,col1.y,col2.y,col0.z,col1.z,col2.z) {}
	mat<3,3,T> &operator = ( const mat<3,3,T> &m ) { for ( size_t i = 0; i < 3; ++i ) { for ( size_t j = 3; j--; data_[i][j] = m.data_[i][j] ); } return *this; }
	T& operator()(const size_t i, const size_t j)       { assert(i<3); assert(j<3); return data_[i][j]; }
	const T& operator()(const size_t i, const size_t j) const { assert(i<3); assert(j<3); return data_[i][j]; }
	static mat<3,3,T> identity() { mat<3,3,T> res; for ( size_t i = 3; i--; res(i,i) = (T)1 ); return res; }
	static mat<3,3,T> zero() { return mat<3,3,T>(); }
	static mat<3,3,T> rotation(const vec<3,T>& angle); // Rotation as R=Rz*Ry*Rx, where Ra is rotation around axis a.
	static mat<3,3,T> diagonalize( const vec<3,T>& diagonal ) { return mat<3,3,T>(
		diagonal.x, 0, 0,
		0, diagonal.y, 0,
		0, 0, diagonal.z); }
	static mat<3,3,T> diagonalize( const T& x, const T& y, const T& z ) { return mat<3,3,T>(
		x, 0, 0,
		0, y, 0,
		0, 0, z); }
	vec<3,T> row(const size_t i) const { vec<3,T> v; for( size_t j = 3; j--; v[j] = data_[i][j] ); return v; }
	vec<3,T> column(const size_t j) const { vec<3,T> v; for( size_t i = 3; i--; v[i] = data_[i][j] ); return v; }
	mat<3,3,T> inverse() const;
private:
	T data_[3][3];
};


template <typename T> struct mat<4,4,T>
{
	mat() { for ( size_t i = 4; i--; ) { for ( size_t j = 4; j--; data_[i][j] = T() ); } }
	mat( const T& m00, const T& m01, const T& m02, const T& m03,
		 const T& m10, const T& m11, const T& m12, const T& m13,
		 const T& m20, const T& m21, const T& m22, const T& m23,
		 const T& m30, const T& m31, const T& m32, const T& m33)
		{ (*this)(0,0) = m00; (*this)(0,1) = m01; (*this)(0,2) = m02; (*this)(0,3) = m03;
		  (*this)(1,0) = m10; (*this)(1,1) = m11; (*this)(1,2) = m12; (*this)(0,3) = m13;
		  (*this)(2,0) = m20; (*this)(2,1) = m21; (*this)(2,2) = m22; (*this)(0,3) = m23;
		  (*this)(3,0) = m30; (*this)(3,1) = m31; (*this)(3,2) = m32; (*this)(3,3) = m33; }
	mat( const vec<4,T>& col0, const vec<4,T>& col1, const vec<4,T>& col2, const vec<4,T>& col3 ) : mat(col0.x,col1.x,col2.x,col3.x,col0.y,col1.y,col2.y,col3.y,col0.z,col1.z,col2.z,col3.z,col0.w,col1.w,col2.w,col3.w) {}
	mat<4,4,T> &operator = ( const mat<4,4,T> &m ) { for ( size_t i = 0; i < 4; ++i ) { for ( size_t j = 4; j--; data_[i][j] = m.data_[i][j] ); } return *this; }
	T& operator()(const size_t i, const size_t j)       { assert(i<4); assert(j<4); return data_[i][j]; }
	const T& operator()(const size_t i, const size_t j) const { assert(i<4); assert(j<4); return data_[i][j]; }
	static mat<4,4,T> identity() { mat<4,4,T> res; for ( size_t i = 4; i--; res(i,i) = (T)1 ); return res; }
	static mat<4,4,T> zero() { return mat<4,4,T>(); }
	static mat<4,4,T> diagonalize( const vec<4,T>& diagonal ) { return mat<4,4,T>(
		diagonal.x, 0, 0, 0,
		0, diagonal.y, 0, 0,
		0, 0, diagonal.z, 0,
		0, 0, 0, diagonal.w); }
	static mat<4,4,T> diagonalize( const T& x, const T& y, const T& z, const T& w ) { return mat<4,4,T>(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, w); }
	vec<4,T> row(const size_t i) const { vec<4,T> v; for( size_t j = 4; j--; v[j] = data_[i][j] ); return v; }
	vec<4,T> column(const size_t j) const { vec<4,T> v; for( size_t i = 4; i--; v[i] = data_[i][j] ); return v; }
	mat<4,4,T> inverse() const;
private:
	T data_[4][4];
};


template <size_t ROWS, size_t COLS, typename T> mat<ROWS, COLS, T> operator + ( const mat<ROWS, COLS, T>& lhs, const mat<ROWS, COLS, T>& rhs );

template <size_t ROWS, size_t COLS, typename T> mat<ROWS, COLS, T> operator - ( const mat<ROWS, COLS, T>& lhs, const mat<ROWS, COLS, T>& rhs );

template <size_t ROWS, size_t COLS, typename T> mat<ROWS, COLS, T> operator * ( const T& lhs, const mat<ROWS, COLS, T>& rhs );

template <size_t ROWS, size_t COLS, typename T> mat<ROWS, COLS, T> operator * ( const mat<ROWS, COLS, T>& lhs, const T& rhs );

template <size_t DIM1, size_t DIM2, size_t DIM3, typename T> mat<DIM1, DIM3, T> operator * ( const mat<DIM1, DIM2, T>& lhs, const mat<DIM2, DIM3, T>& rhs );

template <size_t ROWS, size_t COLS, typename T> vec<ROWS, T> operator * ( const mat<ROWS, ROWS, T>& lhs, const vec<COLS, T>& rhs );

template <size_t ROWS, size_t COLS, typename T> vec<COLS, T> operator * ( const vec<ROWS, T>& lhs, const mat<ROWS, COLS, T>& rhs );

template <size_t DIM, typename T> vec<DIM - 1, T> homo_prod( const mat<DIM, DIM, T>& lhs, const vec<DIM - 1, T>& rhs );



template <typename T> struct affine2
{
	affine2() { rotation = mat<2,2,T>::identity(); translation = vec<2,T>(); }
	affine2( const mat<2,2,T> &rot ) { rotation = rot; translation = vec<2,T>(); }
	affine2( const vec<2,T> &trans ) { rotation = mat<2,2,T>::identity(); translation = trans; }
	affine2( const mat<2,2,T> &rot, const vec<2,T> &trans ) { rotation = rot; translation = trans; }
	affine2<T> operator * (const affine2<T>& transform) const { return affine2<T>( rotation*transform.rotation, rotation*transform.translation+translation ); }
	vec<2,T> operator * (const vec<2,T> &v) const { return rotation*v + translation; }
	affine2<T> inverse() const { mat<2,2,T> invrot = rotation.inverse(); return affine2<T>( invrot, -(invrot*translation) ); }
	mat<3,3,T> toMat33() const { mat<3,3,T> res; for ( size_t i = 0; i < 2; ++i ) { for ( size_t j = 0; j < 2; ++j ) res(i,j) = rotation(i,j); } for ( size_t i = 3; i--; res(2,i) = translation[i] ); res(2,2) = (T)1; }
	static affine2<T> identity() { return affine2<T>( mat<2,2,T>::identity(), vec<2,T>(0,0) ); }
public:
	mat<2,2,T> rotation;
	vec<2,T> translation;
};


template <typename T> struct affine3
{
	affine3() { rotation = mat<3,3,T>::identity(); translation = vec<3,T>(); }
	affine3( const mat<3,3,T> &rot ) { rotation = rot; translation = vec<3,T>(); }
	affine3( const vec<3,T> &trans ) { rotation = mat<3,3,T>::identity(); translation = trans; }
	affine3( const mat<3,3,T> &rot, const vec<3,T> &trans ) { rotation = rot; translation = trans; }
	affine3<T> operator * (const affine3<T>& transform) const { return affine3<T>( rotation*transform.rotation, rotation*transform.translation+translation ); }
	vec<3,T> operator * (const vec<3,T> &v) const { return rotation*v + translation; }
	affine3<T> inverse() const { mat<3,3,T> invrot = rotation.inverse(); return affine3<T>( invrot, -(invrot*translation) ); }
	mat<4,4,T> toMat44() const { mat<4,4,T> res; for ( size_t i = 0; i < 3; ++i ) { for ( size_t j = 0; j < 3; ++j ) res(i,j) = rotation(i,j); } for ( size_t i = 4; i--; res(3,i) = translation[i] ); res(3,3) = (T)1; }
	static affine3<T> identity() { return affine3<T>( mat<3,3,T>::identity(), vec<3,T>(0,0,0) ); }
public:
	mat<3,3,T> rotation;
	vec<3,T> translation;
};









template <size_t ROWS, size_t COLS, typename T> mat<ROWS,COLS,T> operator + (const mat<ROWS,COLS,T>& lhs, const mat<ROWS,COLS,T>& rhs)
{
	mat<ROWS,COLS,T> result;
	for ( size_t i = 0; i < ROWS; ++i )
	{
		for ( size_t j = 0; j < COLS; ++j )
		{
			result(i,j) = lhs(i,j) + rhs(i,j);
		}
	}
	return result;
}


template <size_t ROWS, size_t COLS, typename T> mat<ROWS,COLS,T> operator - (const mat<ROWS,COLS,T>& lhs, const mat<ROWS,COLS,T>& rhs)
{
	mat<ROWS,COLS,T> result;
	for ( size_t i = 0; i < ROWS; ++i )
	{
		for ( size_t j = 0; j < COLS; ++j )
		{
			result(i,j) = lhs(i,j) - rhs(i,j);
		}
	}
	return result;
}


template <size_t ROWS, size_t COLS, typename T> mat<ROWS,COLS,T> operator * (const T& lhs, const mat<ROWS,COLS,T>& rhs)
{
	mat<ROWS,COLS,T> result;
	for ( size_t i = 0; i < ROWS; ++i )
	{
		for ( size_t j = 0; j < COLS; ++j )
		{
			result(i,j) = lhs * rhs(i,j);
		}
	}
	return result;
}


template <size_t ROWS, size_t COLS, typename T> mat<ROWS,COLS,T> operator * (const mat<ROWS,COLS,T>& lhs, const T& rhs)
{
	mat<ROWS,COLS,T> result;
	for ( size_t i = 0; i < ROWS; ++i )
	{
		for ( size_t j = 0; j < COLS; ++j )
		{
			result(i,j) = lhs(i,j) * rhs;
		}
	}
	return result;
}


template <size_t DIM1, size_t DIM2, size_t DIM3, typename T> mat<DIM1,DIM3,T> operator * (const mat<DIM1,DIM2,T> &lhs, const mat<DIM2,DIM3,T>& rhs)
{
	mat<DIM1,DIM3,T> result;
	for ( size_t i = 0; i < DIM1; ++i )
	{
		for ( size_t j = 0; j < DIM3; ++j )
		{
			result(i,j) = T(0);
			for ( size_t k = DIM2; k--; result(i,j) += lhs(i,k) * rhs(k,j) );
		}
	}
	return result;
}


template <size_t ROWS, size_t COLS, typename T> vec<ROWS,T> operator * (const mat<ROWS,ROWS,T> &lhs, const vec<COLS,T>& rhs)
{
	vec<ROWS,T> result;
	for ( size_t i = 0; i < ROWS; ++i )
	{
		for ( size_t j = COLS; j--; result[i] += lhs(i,j) * rhs[j] );
	}
	return result;
}


template <size_t ROWS, size_t COLS, typename T> vec<COLS,T> operator * (const vec<ROWS,T>& lhs, const mat<ROWS,COLS,T> &rhs)
{
	vec<COLS,T> result;
	for ( size_t i = 0; i < ROWS; ++i )
	{
		for ( size_t j = COLS; j--; result[j] += lhs[j] * rhs(i,j) );
	}
	return result;
}


template <size_t DIM, typename T> vec<DIM-1,T> homo_prod (const mat<DIM,DIM,T> &lhs, const vec<DIM-1,T>& rhs)
{
	vec<DIM,T> temp;
	vec<DIM-1,T> result;
	for ( size_t i = 0; i < DIM; ++i )
	{
		for ( size_t j = DIM-1; j--; temp[i] += lhs(i,j) * rhs[j] );
		temp[i] += lhs(i,DIM-1);
	}
	for ( size_t i = DIM-1; i--; result[i] = temp[i] / temp[DIM-1] );
	return result;
}


template<typename T> inline mat<2,2,T> mat<2,2,T>::rotation( const T& angle )
{
	const T cos = std::cos(angle);
	const T sin = std::sin(angle);
	return mat<2,2,T>( cos, -sin, sin, cos );
}


template<typename T> inline mat<2,2,T> mat<2,2,T>::inverse() const
{
	const mat<2,2,T> &m = *this;
	mat<2,2,T> result;
	const T det = m(0,0)*m(1,1) - m(0,1)*m(1,0);
	result(0,0) =  m(1,1); result(0,1) = -m(0,1);
	result(1,0) = -m(1,0); result(1,1) =  m(0,0);
	result = result * (1/det);
	return result;
}


template<typename T>
inline mat<3,3,T> mat<3,3,T>::rotation( const vec<3,T>& angle )
{
	const T cosX = std::cos(angle.x);
	const T sinX = std::sin(angle.x);
	const T cosY = std::cos(angle.y);
	const T sinY = std::sin(angle.y);
	const T cosZ = std::cos(angle.z);
	const T sinZ = std::sin(angle.z);
	return mat<3,3,T>(
		cosY*cosZ, sinX*sinY*cosZ - cosX*sinZ, cosX*sinY*cosZ + sinX*sinZ,
		cosY*sinZ, sinX*sinY*sinZ + cosX*cosZ, cosX*sinY*sinZ - sinX*cosZ,
		-sinY, sinX*cosY, cosX*cosY );
}

template<typename T> inline mat<3,3,T> mat<3,3,T>::inverse() const
{
	const mat<3,3,T> &m = *this;
	mat<3,3,T> result;
	const T det =
		m(0,0) * ( m(1,1)*m(2,2) - m(2,1)*m(1,2) ) -
		m(0,1) * ( m(1,0)*m(2,2) - m(1,2)*m(2,0) ) +
		m(0,2) * ( m(1,0)*m(2,1) - m(1,1)*m(2,0) );
	const T invdet = 1 / det;
	result(0,0) = ( m(1,1)*m(2,2) - m(2,1)*m(1,2) ) * invdet;
	result(0,1) = ( m(0,2)*m(2,1) - m(0,1)*m(2,2) ) * invdet;
	result(0,2) = ( m(0,1)*m(1,2) - m(0,2)*m(1,1) ) * invdet;
	result(1,0) = ( m(1,2)*m(2,0) - m(1,0)*m(2,2) ) * invdet;
	result(1,1) = ( m(0,0)*m(2,2) - m(0,2)*m(2,0) ) * invdet;
	result(1,2) = ( m(1,0)*m(0,2) - m(0,0)*m(1,2) ) * invdet;
	result(2,0) = ( m(1,0)*m(2,1) - m(2,0)*m(1,1) ) * invdet;
	result(2,1) = ( m(2,0)*m(0,1) - m(0,0)*m(2,1) ) * invdet;
	result(2,2) = ( m(0,0)*m(1,1) - m(1,0)*m(0,1) ) * invdet;
	return result;
}


template<typename T> inline mat<4,4,T> mat<4,4,T>::inverse() const
{
	// See https://graphics.stanford.edu/~mdfisher/Code/Engine/Matrix4.cpp.html
	const mat<4,4,T> &m = *this;
	mat<4,4,T> result;
	T tmp[12]; /* temp array for pairs */
	T src[16]; /* array of transpose source matrix */
			   // transpose matrix 
	for (size_t i = 0; i < 4; i++)
	{
		src[i+0 ] = m(i,0);
		src[i+4 ] = m(i,1);
		src[i+8 ] = m(i,2);
		src[i+12] = m(i,3);
	}
	// calculate pairs for first 8 elements (cofactors) 
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	result(0,0) = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	result(0,0) -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	result(0,1) = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	result(0,1) -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	result(0,2) = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	result(0,2) -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	result(0,3) = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	result(0,3) -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	result(1,0) = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	result(1,0) -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	result(1,1) = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	result(1,1) -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	result(1,2) = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	result(1,2) -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	result(1,3) = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	result(1,3) -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
	// calculate pairs for second 8 elements (cofactors)
	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];
	// calculate second 8 elements (cofactors)
	result(2,0) = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	result(2,0) -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	result(2,1) = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	result(2,1) -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	result(2,2) = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	result(2,2) -= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	result(2,3) = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	result(2,3) -= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	result(3,0) = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	result(3,0) -= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	result(3,1) = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	result(3,1) -= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	result(3,2) = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	result(3,2) -= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	result(3,3) = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	result(3,3) -= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
	// calculate determinant
	const T det=src[0]*result(0,0) + src[1] * result(0,1) + src[2]*result(0,2) + src[3]*result(0,3);
	// calculate matrix inverse
	result = result * (1/det);
	return result;
}



} // namespace LFOptics