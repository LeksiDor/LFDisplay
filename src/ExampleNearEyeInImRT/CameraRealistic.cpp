#include "CameraRealistic.h"


namespace LFOptics
{


CameraRealistic::CameraRealistic()
{
	m_SamplesInAperture.resize(1);
	m_SamplesInAperture[0] = Vec2(0,0);
}


CameraRealistic::~CameraRealistic()
{
}


bool CameraRealistic::SetApertureDiameter(const Real& apertureDiameter)
{
	if ( apertureDiameter <= 0 )
		return false;
	m_ApertureDiameter = apertureDiameter;
	UpdateSamples();
	return true;
}


bool CameraRealistic::SetApertureShape(const ApertureShape& apertureShape)
{
	if ( apertureShape != ApertureShape::Square &&
		 apertureShape != ApertureShape::Disk )
		return false;
	m_ApertureShape = apertureShape;
	UpdateSamples();
	return true;
}


bool CameraRealistic::SetNumSamplesInApertureRow(const Int numSamplesInApertureRow)
{
	if (numSamplesInApertureRow <= 0 )
		return false;
	m_NumSamplesInApertureRow = numSamplesInApertureRow;
	UpdateSamples();
	return true;
}


bool CameraRealistic::SetNumSamplesInPixelRow(const Int numSamplesInPixelRow)
{
	if (numSamplesInPixelRow <= 0)
		return false;
	m_NumSamplesInPixelRow = numSamplesInPixelRow;
	UpdateSamples();
	return true;
}


bool CameraRealistic::SetCameraPositioning(const Affine3& positioning)
{
	m_CameraToWorld = positioning;
	m_WorldToCamera = positioning.inverse();
	return true;
}


Int CameraRealistic::NumSamples() const
{
	return Int( m_SamplesInAperture.size() );
}


void CameraRealistic::GenerateRays(std::vector<Vec4>& colorWeights, std::vector<Ray3>& rays, const Vec2i& imageCoord) const
{
	const Int numSamples = NumSamples();
	colorWeights.resize(numSamples);
	rays.resize(numSamples);

	//const Vec2 imagePlanePos = Vec2(
	//	m_ImagePlaneRange.min.x + (Real(imageCoord.x)+0.5)/Real(m_ImageResolution.x) * (m_ImagePlaneRange.max.x - m_ImagePlaneRange.min.x),
	//	m_ImagePlaneRange.min.y + (Real(imageCoord.y)+0.5)/Real(m_ImageResolution.y) * (m_ImagePlaneRange.max.y - m_ImagePlaneRange.min.y));

	const Real imagePlaneZ = m_ImagePlaneZ < 0 ? m_ImagePlaneZ : -m_ImagePlaneZ;

	for ( Int sampleInd = 0; sampleInd < numSamples; ++sampleInd )
	{
		const Vec2& aperturePlanePos = m_SamplesInAperture[sampleInd];
		const Vec2& inpixelPos = m_SamplesInPixel[sampleInd];
		const Vec2 imagePlanePos = m_ImagePlaneRange.min + (imageCoord.toType<Real>() + inpixelPos) / m_ImageResolution.toType<Real>() * (m_ImagePlaneRange.max - m_ImagePlaneRange.min);
		const Vec2 rayDir2LocalInitial = (aperturePlanePos - imagePlanePos) / -imagePlaneZ;
		const Vec2 rayDir2LocalRefracted = rayDir2LocalInitial - m_ObjectiveInvFocalLength * aperturePlanePos;
		const Vec3 rayOriLocal = Vec3(aperturePlanePos,0);
		const Vec3 rayDirLocal = Vec3(rayDir2LocalRefracted,1).normalized();
		const Ray3 rayLocal = Ray3(rayOriLocal, rayDirLocal);
		const Ray3 rayGlobal = m_CameraToWorld * rayLocal;
		
		rays[sampleInd] = rayGlobal;
		colorWeights[sampleInd] = Vec4(1,1,1,1);
	}
}


void CameraRealistic::UpdateSamples()
{
	const Int numSamplesMax = m_NumSamplesInApertureRow * m_NumSamplesInApertureRow * m_NumSamplesInPixelRow * m_NumSamplesInPixelRow;
	if (numSamplesMax <= 1 )
	{
		m_SamplesInAperture.resize(1);
		m_SamplesInAperture[0] = Vec2(0,0);
		m_SamplesInPixel.resize(1);
		m_SamplesInPixel[0] = Vec2(0.5,0.5);
		return;
	}

	Int samplesCount = 0;
	m_SamplesInAperture.resize(numSamplesMax);
	m_SamplesInPixel.resize(numSamplesMax);
	for ( Int xApertureInd = 0; xApertureInd < m_NumSamplesInApertureRow; ++xApertureInd )
	{
		for ( Int yApertureInd = 0; yApertureInd < m_NumSamplesInApertureRow; ++yApertureInd )
		{
			for ( Int xPixelInd = 0; xPixelInd < m_NumSamplesInPixelRow; ++xPixelInd )
			{
				for ( Int yPixelInd = 0; yPixelInd < m_NumSamplesInPixelRow; ++yPixelInd )
				{
					const Vec2 normalizedAperturePos = Vec2(
						-0.5 + (Real(xApertureInd) + 0.5) / Real(m_NumSamplesInApertureRow),
						-0.5 + (Real(yApertureInd) + 0.5) / Real(m_NumSamplesInApertureRow));
					if (m_ApertureShape == ApertureShape::Disk)
					{
						const Real radius = normalizedAperturePos.norm();
						if (radius >= 0.5)
							continue;
					}
					const Vec2 aperturePos = m_ApertureDiameter * normalizedAperturePos;
					const Vec2 pixelPos = Vec2(
						(Real(xPixelInd) + 0.5) / Real(m_NumSamplesInPixelRow),
						(Real(yPixelInd) + 0.5) / Real(m_NumSamplesInPixelRow));
					m_SamplesInAperture[samplesCount] = aperturePos;
					m_SamplesInPixel[samplesCount] = pixelPos;
					++samplesCount;
				}
			}
		}
	}
	m_SamplesInAperture.resize(samplesCount);
	m_SamplesInPixel.resize(samplesCount);
}


} // namespace LFOptics