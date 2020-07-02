#include "DisplayProjectorsShow.h"

#include "DisplayProjectorAligned.h"
#include "RayGenPinhole.h"
#include "SampleAccumCV.h"
#include "SampleGenUniform.h"

#include "Image.h"


#include <cstdlib>


using ss = std::stringstream;



DisplayProjectorsShow::DisplayProjectorsShow( const DisplayProjectorAligned* displayModel )
	:DisplayModel(displayModel)
{
}


bool DisplayProjectorsShow::LoadScene( const std::string& filepath )
{
	if (DisplayModel == nullptr)
		return false;
	DisplayModel->FillProjectorsPositions(ProjectorPositions);
	const Int numProjectorsTotal = ProjectorPositions.size();
	ProjectorImages.resize(numProjectorsTotal);
	bool success = true;
	for (int i = 0; i < numProjectorsTotal; ++i)
	{
		const std::string image_filepath = (ss() << filepath << "/" << std::setfill('0') << std::setw(4) << i << ".exr").str();
		success = success && LoadImageRGB( image_filepath, ProjectorImages[i] );
	}
	return success;
}


lfrt::RayGenerator* DisplayProjectorsShow::CreateDefaultRayGenerator( const Int& width, const Int& height ) const
{
	if ( DisplayModel == nullptr )
	{
		return new RayGenPinhole( width, height );
	}
	else
	{
		const Vec2 halfSize = DisplayModel->HalfPhysSize;
		const Real dist = DisplayModel->ViewerDistance;
		return new RayGenPinhole( width, height, halfSize[0], halfSize[1], dist );
	}
}


lfrt::SampleGenerator* DisplayProjectorsShow::CreateDefaultSampleGenerator( const Int& width, const Int& height ) const
{
	return new SampleGenUniform(3);
}


lfrt::SampleAccumulator* DisplayProjectorsShow::CreateDefaultSampleAccumulator( const Int& width, const Int& height ) const
{
	return new SampleAccumCV( width, height );
}


bool DisplayProjectorsShow::Render( const lfrt::RayGenerator& raygen, const lfrt::SampleGenerator& sampleGen, lfrt::SampleAccumulator& sampleAccum ) const
{
	// ToDo: display with front-projectors.

	if ( DisplayModel == nullptr )
		return false;

	const Int width  = DisplayModel->ProjectorResolution[0];
	const Int height = DisplayModel->ProjectorResolution[1];

	if ( width <= 0 || height <= 0 )
		return false;
	if ( ProjectorImages.size() == 0 )
		return false;
	if ( ProjectorImages.size() != ProjectorPositions.size() )
		return false;
	for ( auto image = ProjectorImages.begin(); image != ProjectorImages.end(); ++image )
	{
		if ( image->cols != width || image->rows != height || image->type() != CV_32FC3 )
			return false;
	}

	Int globStartX;
	Int globStartY;
	Int globEndX;
	Int globEndY;
	if ( !sampleAccum.GetRenderBounds( globStartX, globStartY, globEndX, globEndY ) )
		return false;
	if ( globStartX < 0 || globStartX >= globEndX ||
		globStartY < 0 || globStartY >= globEndY )
		return false;

	const Int globSizeX = globEndX - globStartX;
	const Int globSizeY = globEndY - globStartY;

	const Int tileSize = 16;

	const Int numTilesX = (globSizeX + tileSize - 1) / tileSize;
	const Int numTilesY = (globSizeY + tileSize - 1) / tileSize;

	const Int numProjectorsTotal = ProjectorPositions.size();
	const Real viewerDistance = DisplayModel->ViewerDistance;
	const Real halfSizeX = DisplayModel->HalfPhysSize[0];
	const Real halfSizeY = DisplayModel->HalfPhysSize[1];
	const Real diffusionRho = DisplayModel->DiffusionPower[0];
	const Real diffusionEta = DisplayModel->DiffusionPower[1];

	std::function<Real(const Real&,const Real&,const Real&)> RhoFunc;
	std::function<Real(const Real&,const Real&,const Real&)> EtaFunc;
	switch ( DisplayModel->DiffuserType )
	{
	case DisplayProjectorAligned::Diffuser::Conical:
		RhoFunc = RhoConical;
		EtaFunc = EtaConical;
		break;
	case DisplayProjectorAligned::Diffuser::Linear:
	default:
		RhoFunc = RhoLinear;
		EtaFunc = EtaLinear;
		break;
	}

	cv::parallel_for_( cv::Range( 0, numTilesX*numTilesY ),
		[&]( const cv::Range& range )
		{
			std::unique_ptr<lfrt::SampleGenerator> sampler( sampleGen.Clone() );

			Real weightSample;
			Real weightRay;
			lfrt::VEC2 raster;
			lfrt:: VEC2 secondary;
			Real time;
			lfrt::VEC3 ori;
			lfrt::VEC3 dir;
			Real r, g, b;

			for ( int tileInd = range.start; tileInd < range.end; ++tileInd )
			{
				const Int tileIndX = tileInd % numTilesX;
				const Int tileIndY = tileInd / numTilesX;

				const Int tileStartX = globStartX + tileIndX * tileSize;
				const Int tileStartY = globStartY + tileIndY * tileSize;

				lfrt::SampleTile* tile = sampleAccum.CreateSampleTile(
					tileStartX, tileStartY, tileStartX+tileSize, tileStartY+tileSize );

				for ( Int xLoc = 0; xLoc < tileSize; ++xLoc )
				{
					for ( Int yLoc = 0; yLoc < tileSize; ++yLoc )
					{
						const Int x = tileStartX + xLoc;
						const Int y = tileStartY + yLoc;

						if ( x >= width || y >= height )
							continue;

						sampler->ResetPixel( x, y );

						do
						{
							if ( !sampler->CurrentSample( weightSample, raster, secondary, time ) )
								continue;
							weightRay = raygen.GenerateRay( raster, secondary, ori, dir );
							if ( weightRay == 0 )
								continue;

							if ( dir.z <= 0 )
								continue;

							// Find ray-screen intersection.
							const Real z0 = viewerDistance;
							const Real x0 = ori.x + (z0 - ori.z) * dir.x / dir.z;
							const Real y0 = ori.y + (z0 - ori.z) * dir.y / dir.z;

							// Find (rho,eta) for viewer ray.
							const Real rhoViewer = RhoFunc( dir.x, dir.y, dir.z );
							const Real etaViewer = EtaFunc( dir.x, dir.y, dir.z );

							const Real texLambdaX = 0.5 * (1.0 + x0/halfSizeX);
							const Real texLambdaY = 0.5 * (1.0 - y0/halfSizeY);

							if ( texLambdaX < 0 || texLambdaX > 1.0 ||
								 texLambdaY < 0 || texLambdaY > 1.0 )
								continue;

							const Int xProj = std::min<Int>( std::max<Int>( Int(texLambdaX*width) , 0 ), width-1 );
							const Int yProj = std::min<Int>( std::max<Int>( Int(texLambdaY*height), 0 ), height-1 );

							// Iterate over all projectors and add the contribution of each projector.
							Real weightSum = 0.0;
							Color colorSum = Color(0,0,0);
							for ( Int projInd = 0; projInd < numProjectorsTotal; ++projInd )
							{
								const Vec3 projPos = ProjectorPositions[projInd];
								const Real dx = projPos[0] - x0;
								const Real dy = projPos[1] - y0;
								const Real dz = projPos[2] - z0;
								// Find (rho,eta) for projector ray.
								const Real rhoProj = RhoFunc( dx, dy, dz );
								const Real etaProj = EtaFunc( dx, dy, dz );

								const Real rhoDiff = rhoProj - rhoViewer;
								const Real etaDiff = etaProj - etaViewer;

								const Real weightRho = std::exp( -diffusionRho*rhoDiff*rhoDiff );
								const Real weightEta = std::exp( -diffusionEta*etaDiff*etaDiff );
								const Real weight = weightRho * weightEta;

								if ( weight >= 0.00001 )
								{
									const Color projColor = ProjectorImages[projInd].at<Color>(yProj,xProj);
									colorSum += weight*projColor;
									weightSum += weight;
								}
							}

							if ( weightSum >= 0.00001 )
							{
								r = colorSum[2] / weightSum;
								g = colorSum[1] / weightSum;
								b = colorSum[0] / weightSum;
							}
							else
							{
								r = 0;
								g = 0;
								b = 0;
							}

							tile->AddSample( raster, secondary, weightSample, weightRay, r, g, b );
						}
						while ( sampler->MoveToNextSample() );
					}
				}

				sampleAccum.MergeSampleTile( tile );
				sampleAccum.DestroySampleTile( tile );
			}
		}
	);

	return true;
}


Real DisplayProjectorsShow::RhoConical( const Real& dx, const Real& dy, const Real& dz )
{
	return dx / std::sqrt(dy*dy + dz*dz);
}


Real DisplayProjectorsShow::EtaConical(const Real& dx, const Real& dy, const Real& dz)
{
	return dy / dz;
}


Real DisplayProjectorsShow::RhoLinear(const Real& dx, const Real& dy, const Real& dz)
{
	return dx / dz;
}


Real DisplayProjectorsShow::EtaLinear(const Real& dx, const Real& dy, const Real& dz)
{
	return dy / dz;
}
