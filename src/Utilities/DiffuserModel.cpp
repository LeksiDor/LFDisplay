#include "DiffuserModel.h"

#include "DisplayProjectorAligned.h"


class DiffuserFlat : public DiffuserModel
{
public:

	virtual Real FindMaxOnViewerLine( const Vec3& rayOrigin, const Vec3& intersection ) const override
	{
		const Real x0 = intersection[0];
		const Real y0 = intersection[1];
		const Real z0 = intersection[2];

		const Real dx = intersection[0] - rayOrigin[0];
		const Real dy = intersection[1] - rayOrigin[1];
		const Real dz = intersection[2] - rayOrigin[2];

		if ( IsConicalRefraction )
		{
			// (X,0,0) must be the intersection between cone (x-x0)^2=rho^2*((y-y0)^2+(z-z0)^2) with observer line (x,0,0).
			// In other words, we solve equation (X-x0)^2=rho^2 * (y0^2 + z0^2).
			// Variable rho is determined as rho = dx / sqrt(dy^2 + dz^2).
			const Real rho = dx / std::sqrt(dy*dy+dz*dz);
			const Real X = x0 + rho*std::sqrt(y0*y0+z0*z0);
			return X;
		}
		else
		{
			// X is determined as the intersection between 2D lines (x,0) and (x0,z0)+t*(dx,dz).
			const Real X = x0 - z0*dx/dz;
			return X;
		}
	}

	virtual Real RefractedIntensity( const Vec3& rayOrigin, const Vec3& intersection, const Vec3& destination ) const override
	{
		const Real dx1 = rayOrigin[0] - intersection[0];
		const Real dy1 = rayOrigin[1] - intersection[1];
		const Real dz1 = rayOrigin[2] - intersection[2];

		const Real dx2 = destination[0] - intersection[0];
		const Real dy2 = destination[1] - intersection[1];
		const Real dz2 = destination[2] - intersection[2];

		Real rho1, rho2, eta1, eta2;
		if ( IsConicalRefraction )
		{
			const Real sign1 = dz1 > 0 ? 1.0 : -1.0;
			const Real sign2 = dz2 > 0 ? 1.0 : -1.0;
			rho1 = sign1 * dx1 / std::sqrt(dy1*dy1 + dz1*dz1);
			rho2 = sign2 * dx2 / std::sqrt(dy2*dy2 + dz2*dz2);
			eta1 = dy1 / dz1;
			eta2 = dy2 / dz2;
		}
		else
		{
			rho1 = dx1 / dz1;
			rho2 = dx2 / dz2;
			eta1 = dy1 / dz1;
			eta2 = dy2 / dz2;
		}

		const Real diffRho = rho1 - rho2;
		const Real diffEta = eta1 - eta2;
		const Real weightRho = std::exp( -DiffusionPower[0]*diffRho*diffRho );
		const Real weightEta = std::exp( -DiffusionPower[1]*diffEta*diffEta );
		const Real weight = weightRho * weightEta;
		return weight;
	}

public:
	Real ScreenZ = 1.0; // Z-coordinate of the screen plane.
	Vec2 DiffusionPower = Vec2(100,0); // Inverse angular scattering power. Zero for ideal uniform diffusion.
	bool IsConicalRefraction = true; // Refraction model.
};





DiffuserModel* DiffuserModel::Create( const DisplayProjectorAligned& displayModel )
{
	DiffuserFlat* diffuser = new DiffuserFlat();
	diffuser->ScreenZ = displayModel.ViewerDistance;
	diffuser->DiffusionPower = displayModel.DiffusionPower;
	diffuser->IsConicalRefraction = (displayModel.DiffuserType == DisplayProjectorAligned::Diffuser::Conical);
	return diffuser;
}
