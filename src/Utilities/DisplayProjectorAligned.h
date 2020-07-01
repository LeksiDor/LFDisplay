#ifndef DISPLAYPROJECTORALIGNED_H
#define DISPLAYPROJECTORALIGNED_H

#include "BaseTypes.h"

// Data storage for projector-based display.
// Assumptions:
//     * Screen surface is flat.
//     * On-screen pixel grid from all projectors is the same.
// Display geometry:
// Observer line: ( x, 0, 0 ).
// Observer plane: ( x, y, 0 ).
// Screen plane: ( x, y, ViewerDistance ).
// Projector line: start + i*step, i=0,..,number-1.
class DisplayProjectorAligned
{
public:
	enum class Diffuser
	{
		Linear,
		Conical,
	};

	struct ProjectorLine
	{
		Vec3 start; // Starting point of the projector array.
		Vec3 step; // Difference between the current and previous projectors.
		Int number; // Total number of projectors on the line.
	};


public:

	bool Save( const std::string& filepath ) const;
	bool Load( const std::string& filepath );

	void FillProjectorsPositions( std::vector<Vec3>& positions ) const;

public:
	Real ViewerDistance = Real(1); // Physical distance from viewer to the screen center.
	Vec2i ProjectorResolution = Vec2i(1,1); // Resolution of each individual projector.
	Vec2 HalfPhysSize = Vec2(1,1); // Half of physical size.
	std::vector<ProjectorLine> ProjectorLines;
	Diffuser DiffuserType = Diffuser::Linear;
	Vec2 DiffusionPower = Vec2(100,0); // Inverse angular scattering power. Zero for ideal uniform diffusion.
};

#endif // DISPLAYPROJECTORALIGNED_H