#include "DisplayProjectorsCapture.h"

#include "DiffuserModel.h"
#include "DisplayProjectorAligned.h"



DisplayProjectorsCapture::DisplayProjectorsCapture( const DisplayProjectorAligned* model, const Vec3& projectorPos )
	:m_DisplayModel(model)
	,m_ProjectorPosition(projectorPos)
{
	m_DiffuserModel.reset( DiffuserModel::Create(*model) );
}


Real DisplayProjectorsCapture::GenerateRay( const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir ) const
{
	if ( m_DisplayModel == nullptr )
		return 0;

	const Real lambdaX = -1.0 + 2.0 * raster.x / (Real)m_DisplayModel->ProjectorResolution[0];
	const Real lambdaY =  1.0 - 2.0 * raster.y / (Real)m_DisplayModel->ProjectorResolution[1];

	// Screen position.
	const Real x0 = lambdaX * m_DisplayModel->HalfPhysSize[0];
	const Real y0 = lambdaY * m_DisplayModel->HalfPhysSize[1];
	const Real z0 = m_DisplayModel->ViewerDistance;

	// Viewer position on the observer line.
	const Real X = m_DiffuserModel->FindMaxOnViewerLine( m_ProjectorPosition, Vec3(x0,y0,z0) );

	ori.x = X;
	ori.y = 0;
	ori.z = 0;
	dir.x = x0 - X;
	dir.y = y0;
	dir.z = z0;

	return 1;
}
