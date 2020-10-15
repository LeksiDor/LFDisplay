#ifndef UTILITIES_DIFFUSERTANBASED_H
#define UTILITIES_DIFFUSERTANBASED_H

#include "DiffuserModel.h"


class DiffuserTanBased : public DiffuserModel
{
public:
	virtual ~DiffuserTanBased() = default;

	virtual Real FindMaxOnLine( const Vec3& rayOri, const Vec3& lineOri, const Vec3& lineDir = Vec3(1,0,0) ) const override;

	virtual Real Diffusion( const Vec3& pointA, const Vec3& pointB ) const override;


	Vec2 DiffusionPower = Vec2(100,0); // Inverse angular scattering power. Zero for ideal uniform diffusion, infinity for Dirac.
	Real Normalizer = 1.0; // Normalizer of diffusion coefficients.
};


#endif // UTILITIES_DIFFUSERTANBASED_H