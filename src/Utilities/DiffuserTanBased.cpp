#include "DiffuserTanBased.h"



Real DiffuserTanBased::FindMaxOnLine( const Vec3& rayOri, const Vec3& lineOri, const Vec3& lineDir ) const
{
	// Scattering cone is x^2=rho^2*(y^2+z^2).
	// Determine 'rho' as rho=x/sqrt(y^2+z^2).
	const Real signRay  = rayOri(2)  < 0 ? -1 : 1;
	const Real signLine = lineOri(2) < 0 ? -1 : 1;
	const Real rho = signRay * rayOri(0) / std::sqrt( rayOri(1)*rayOri(1) + rayOri(2)*rayOri(2) );
	// Now, find the intersection between cone x^2=rho^2*(y^2+z^2)
	// and line (x,y,z)=e+lambda*d, where 'lambda' is unknown.
	const Vec3 e = lineOri;
	const Vec3 d = lineDir;
	// Compose and solve quadratic equation: a*lambda^2 + 2*b*lambda + c = 0.
	Real lambdaA = 0;
	Real lambdaB = 0;
	if ( d == Vec3(1,0,0) )
	{
		// For case d==(1,0,0), equation is simple.
		const Real lambdaDiff = rho*std::sqrt( e(1)*e(1) + e(2)*e(2) );
		lambdaA = -e(0) + lambdaDiff;
		lambdaB = -e(0) - lambdaDiff;
	}
	else
	{
		const Real a = d(0)*d(0) - rho*rho*( d(1)*d(1) + d(2)*d(2) );
		const Real b = d(0)*e(0) - rho*rho*( d(1)*e(1) + d(2)*e(2) );
		const Real c = e(0)*e(0) - rho*rho*( e(1)*e(1) + e(2)*e(2) );
		const Real D = b*b - a*c;
		if ( D < 0 )
			return 0;
		const Real sqrtD = std::sqrt(D);
		lambdaA = (-b + sqrtD) / a;
		lambdaB = (-b - sqrtD) / a;
		
	}
	// Choose the one which gives the same 'rho'.
	const Vec3 posA = e + lambdaA*d;
	const Vec3 posB = e + lambdaB*d;
	const Real signA = posA(2) < 0 ? -1 : 1;
	const Real signB = posB(2) < 0 ? -1 : 1;
	if ( signA == signLine && signB != signLine )
		return lambdaA;
	else if ( signA != signLine && signB == signLine )
		return lambdaB;
	else
	{
		const Real rhoA = signA * posA(0) / std::sqrt( posA(1)*posA(1) + posA(2)*posA(2) );
		const Real rhoB = signB * posB(0) / std::sqrt( posB(1)*posB(1) + posB(2)*posB(2) );
		if ( std::abs(rhoA-rho) < std::abs(rhoB-rho) )
			return lambdaA;
		else
			return lambdaB;
	}
}



Real DiffuserTanBased::Diffusion( const Vec3& pointA, const Vec3& pointB ) const
{
	const Vec3 dA = pointA;
	const Vec3 dB = pointB;
	const Real signA = dA(2) < 0 ? -1 : 1;
	const Real signB = dB(2) < 0 ? -1 : 1;
	const Real rhoA = signA * dA(0) / std::sqrt( dA(1)*dA(1) + dA(2)*dA(2) );
	const Real rhoB = signB * dB(0) / std::sqrt( dB(1)*dB(1) + dB(2)*dB(2) );
	const Real etaA = dA(1) / dA(2);
	const Real etaB = dB(1) / dB(2);
	const Real diffRho = rhoA - rhoB;
	const Real diffEta = etaA - etaB;
	const Real expArg = -DiffusionPower(0)*diffRho*diffRho - DiffusionPower(1)*diffEta*diffEta;
	return Normalizer * std::exp( expArg );
}