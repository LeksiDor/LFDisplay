#pragma once

#include "BaseTypes.h"
#include "BaseEnums.h"
#include "BaseInterfaces.h"


namespace LFOptics
{

class CameraRealistic : public IRayGeneratorOnImageMulti
{
public:
	CameraRealistic();
	~CameraRealistic();

	Real& ImagePlaneZ() { return m_ImagePlaneZ; }
	MinMax2Real& ImagePlaneRange() { return m_ImagePlaneRange; }
	Vec2i& ImageResolution() { return m_ImageResolution; }
	Real& ObjectiveInvFocalLength() { return m_ObjectiveInvFocalLength; }

	const Real& ImagePlaneZ() const { return m_ImagePlaneZ; }
	const MinMax2Real& ImagePlaneRange() const  { return m_ImagePlaneRange; }
	const Vec2i& ImageResolution() const { return m_ImageResolution; }
	const Real& ObjectiveInvFocalLength() const  { return m_ObjectiveInvFocalLength; }
	const Real& ApertureDiameter() const  { return m_ApertureDiameter; }
	const ApertureShape& ApertureShapeType() const { return m_ApertureShape; }
	const Int& NumSamplesInApertureRow() const { return m_NumSamplesInApertureRow; }
	const Int& NumSamplesInPixelRow() const { return m_NumSamplesInPixelRow; }

	const Affine3& CameraToWorld() const { return m_CameraToWorld; }
	const Affine3& WorldToCamera() const { return m_WorldToCamera; }

	bool SetApertureDiameter(const Real& apertureDiameter);
	bool SetApertureShape(const ApertureShape& apertureShape);
	bool SetNumSamplesInApertureRow(const Int numSamplesInApertureRow);
	bool SetNumSamplesInPixelRow(const Int numSamplesInPixelRow);
	// Position and orientation of the aperture center.
	// XY-axes -- hor/ver image coordinates; Z-axis -- direction from camera.
	bool SetCameraPositioning(const Affine3& positioning);

	// Inherited via IRayGeneratorOnImageMulti
	virtual Int NumSamples() const override;
	virtual void GenerateRays(std::vector<Vec4>& colorWeights, std::vector<Ray3>& rays, const Vec2i& imageCoord) const override;

private:
	void UpdateSamples();

private:
	Real m_ImagePlaneZ = Real(-1);
	MinMax2Real m_ImagePlaneRange = MinMax2Real(-1, -1, 1, 1);
	Vec2i m_ImageResolution = Vec2i(1,1);
	Real m_ObjectiveInvFocalLength = Real(0);
	Real m_ApertureDiameter = Real(1);
	ApertureShape m_ApertureShape = ApertureShape::Square;

	Int m_NumSamplesInApertureRow = 1;
	Int m_NumSamplesInPixelRow = 1;
	std::vector<Vec2> m_SamplesInAperture;
	std::vector<Vec2> m_SamplesInPixel;

	Affine3 m_CameraToWorld = Affine3(Mat33::identity(), Vec3(0, 0, 1));
	Affine3 m_WorldToCamera = Affine3(Mat33::identity(), Vec3(0, 0, 1));
};

} // namespace LFOptics