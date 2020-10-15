#include "SampleGenDisk.h"

using namespace lfrt;


SampleGenDisk::SampleGenDisk( const Int primaryRes, const Int secondaryRes )
	:primaryRes(0)
	,secondaryRes(0)
{
	SetPrimaryRes( primaryRes );
	SetSecondaryRes( secondaryRes );
}


SampleGenerator* SampleGenDisk::Clone() const
{
	return new SampleGenDisk( primaryRes, secondaryRes );
}


bool SampleGenDisk::ResetPixel( const Int& x, const Int& y )
{
	this->x = x;
	this->y = y;
	currentSampleInd = 0;
	return true;
}


Int SampleGenDisk::NumSamplesInPixel()
{
	return primaryRes*primaryRes*apertureCoords.size();
}


bool SampleGenDisk::CurrentSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time )
{
	Int index = currentSampleInd;
	const Int numApertureCoords = apertureCoords.size();
	const Int indAp = index % numApertureCoords;
	const Int indY = (index /= numApertureCoords) % primaryRes;
	const Int indX = (index /= primaryRes) % primaryRes;
	weight = 1.0;
	const Real dx1 = (0.5+Real(indX)) / Real(primaryRes);
	const Real dy1 = (0.5+Real(indY)) / Real(primaryRes);
	const VEC2 ap = apertureCoords[indAp];
	raster = { Real(x)+dx1, Real(y)+dy1 };
	secondary = ap;
	time = 0.0;
	return true;
}


bool SampleGenDisk::MoveToNextSample()
{
	++currentSampleInd;
	return currentSampleInd < primaryRes*primaryRes*apertureCoords.size();
}


bool SampleGenDisk::SetPrimaryRes( const Int primaryRes )
{
	if ( primaryRes <= 0 )
		return false;
	this->primaryRes = primaryRes;
	return true;
}


bool SampleGenDisk::SetSecondaryRes( const Int secondaryRes )
{
	if ( secondaryRes <= 0 )
		return false;
	this->secondaryRes = secondaryRes;
	apertureCoords.clear();
	for ( Int i = 0; i < secondaryRes; ++i )
	{
		for ( Int j = 0; j < secondaryRes; ++j )
		{
			const Real x = -0.5 + (Real(i)+0.5) / Real(secondaryRes);
			const Real y = -0.5 + (Real(j)+0.5) / Real(secondaryRes);
			if ( x*x + y*y > 0.25 )
				continue;
			apertureCoords.push_back({ x+0.5, y+0.5 });
		}
	}
	return true;
}
