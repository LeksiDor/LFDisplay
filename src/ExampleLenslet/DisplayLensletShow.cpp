#include "DisplayLensletShow.h"

#include "DisplayLenslet.h"
#include "RayGenPinhole.h"
#include "SampleAccumCV.h"
#include "SampleGenUniform.h"

#include "Image.h"

DisplayLensletShow::DisplayLensletShow( const DisplayLenslet* displayModel )
	:DisplayModel(displayModel)
{
}


bool DisplayLensletShow::LoadScene( const std::string& filepath )
{
	return LoadImageRGB( filepath, DisplayImage );
}


lfrt::RayGenerator* DisplayLensletShow::CreateDefaultRayGenerator( const Int& width, const Int& height ) const
{
	if ( DisplayModel == nullptr )
	{
		return new RayGenPinhole( width, height );
	}
	else
	{
		const Real dist = DisplayModel->LensletToOrigin + DisplayModel->LensletToLCD;
		const Vec2 size = DisplayModel->SizeLCD;
		return new RayGenPinhole( width, height, { 0.5*size[0], 0.5*size[1] }, dist );
	}
}


lfrt::SampleGenerator* DisplayLensletShow::CreateDefaultSampleGenerator( const Int& width, const Int& height ) const
{
	return new SampleGenUniform(3);
}


lfrt::SampleAccumulator* DisplayLensletShow::CreateDefaultSampleAccumulator( const Int& width, const Int& height ) const
{
	return new SampleAccumCV( width, height );
}


bool DisplayLensletShow::Render( const lfrt::RayGenerator& raygen, const lfrt::SampleGenerator& sampleGen, lfrt::SampleAccumulator& sampleAccum ) const
{
	if ( DisplayModel == nullptr )
		return false;

	const Int lcdresX = DisplayModel->ResolutionLCD[0];
	const Int lcdresY = DisplayModel->ResolutionLCD[1];
	const Real lcdSizeX = DisplayModel->SizeLCD[0];
	const Real lcdSizeY = DisplayModel->SizeLCD[1];

	if ( DisplayImage.cols != lcdresX || DisplayImage.rows != lcdresY )
		return false;
	if ( DisplayImage.type() != CV_32FC3 )
		return false;

	const Int width = sampleAccum.Width();
	const Int height = sampleAccum.Height();
	if ( width <= 0 || height <= 0 )
		return false;

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

	const Real f = DisplayModel->LensletToOrigin;
	const Real d = DisplayModel->LensletToLCD;

	const bool isLensletVertical = DisplayModel->IsLensletVertical;
	const Vec2& lensletShift = DisplayModel->LensletShift();
	const Mat22& lensletOrientation = DisplayModel->LensletOrientation();
	const Vec2& lensletShiftInv = DisplayModel->LensletShiftInv();
	const Mat22& lensletOrientationInv = DisplayModel->LensletOrientationInv();

	cv::parallel_for_( cv::Range( 0, numTilesX*numTilesY ),
		[&]( const cv::Range& range )
		{
			std::unique_ptr<lfrt::SampleGenerator> sampler( sampleGen.Clone() );

			Real weightSample;
			Real weightRay;
			VEC2 raster;
			VEC2 secondary;
			Real time;
			VEC3 ori;
			VEC3 dir;
			Real r, g, b;

			for ( int r = range.start; r < range.end; ++r )
			{
				const Int tileIndX = r % numTilesX;
				const Int tileIndY = r / numTilesX;

				std::cout << "Tile " << tileIndX << " " << tileIndY << " started." << std::endl;

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

							const Real dirTanX = dir.x / dir.z;
							const Real dirTanY = dir.y / dir.z;

							const Real viewerX = ori.x - dirTanX * ori.z;
							const Real viewerY = ori.y - dirTanY * ori.z;

							const Real lensletX = ori.x + dirTanX * ( f - ori.z );
							const Real lensletY = ori.y + dirTanY * ( f - ori.z );

							const Vec2 lensletIndReal = lensletShiftInv + lensletOrientationInv * Vec2(lensletX,lensletY);
							const Int lensletIndX = std::round( lensletIndReal[0] );
							const Int lensletIndY = std::round( lensletIndReal[1] );

							const Vec2 lensletCenter = lensletShift + lensletOrientation * Vec2(lensletIndX,lensletIndY);

							Real lcdPosX;
							Real lcdPosY;

							if ( isLensletVertical )
							{
								// ToDo
							}
							else
							{
								lcdPosX = viewerX + (f+d)/d*( lensletCenter[0] - viewerX );
								lcdPosY = viewerY + (f+d)/d*( lensletCenter[1] - viewerY );
							}

							const Real lcdLambdaX = 0.5 + lcdPosX / lcdSizeX;
							const Real lcdLambdaY = 0.5 + lcdPosY / lcdSizeY;

							const Real lcdPixelX = lcdLambdaX * lcdresX;
							const Real lcdPixelY = lcdLambdaY * lcdresY;

							if ( lcdPixelX < 0 || lcdPixelX > lcdresX ||
								 lcdPixelY < 0 || lcdPixelY > lcdresY )
								continue;

							Color color = DisplayImage.at<Color>( Int(lcdPixelY), Int(lcdPixelX) );

							r = color[2];
							g = color[1];
							b = color[0];

							tile->AddSample( raster, secondary, weightSample, weightRay, r, g, b );
						}
						while ( sampler->MoveToNextSample() );
					}
				}

				sampleAccum.MergeSampleTile( tile );
				sampleAccum.DestroySampleTile( tile );

				std::cout << "Tile " << tileIndX << " " << tileIndY << " ended." << std::endl;
			}
		}
	);

	return true;
}




bool DisplayLensletShow::Render( const lfrt::RayGenerator& raygen, lfrt::SampleGenerator& sampleGen, cv::Mat& result, const Int width, const Int height ) const
{
	if ( DisplayModel == nullptr )
		return false;

	const Int lcdresX = DisplayModel->ResolutionLCD[0];
	const Int lcdresY = DisplayModel->ResolutionLCD[1];
	const Real lcdSizeX = DisplayModel->SizeLCD[0];
	const Real lcdSizeY = DisplayModel->SizeLCD[1];

	if ( DisplayImage.cols != lcdresX || DisplayImage.rows != lcdresY )
		return false;
	result = cv::Mat::zeros( height, width, CV_32FC3 );

	const Real f = DisplayModel->LensletToOrigin;
	const Real d = DisplayModel->LensletToLCD;

	const bool isLensletVertical = DisplayModel->IsLensletVertical;
	const Vec2& lensletShift = DisplayModel->LensletShift();
	const Mat22& lensletOrientation = DisplayModel->LensletOrientation();
	const Vec2& lensletShiftInv = DisplayModel->LensletShiftInv();
	const Mat22& lensletOrientationInv = DisplayModel->LensletOrientationInv();

	Real weightSample;
	Real weightRay;
	VEC2 raster;
	VEC2 secondary;
	Real time;
	VEC3 ori;
	VEC3 dir;
	//Real r, g, b;

	for ( Int x = 0; x < width; ++x )
	{
		for ( Int y = 0; y < height; ++y )
		{
			sampleGen.ResetPixel( x, y );

			Real weightsSum = 0;
			Color& colorSum = result.at<Color>(y,x);

			do
			{
				if ( !sampleGen.CurrentSample( weightSample, raster, secondary, time ) )
					continue;
				weightRay = raygen.GenerateRay( raster, secondary, ori, dir );
				if ( weightRay == 0 )
					continue;

				if ( dir.z <= 0 )
					continue;

				const Real dirTanX = dir.x / dir.z;
				const Real dirTanY = dir.y / dir.z;

				const Real viewerX = ori.x - dirTanX * ori.z;
				const Real viewerY = ori.y - dirTanY * ori.z;

				const Real lensletX = ori.x + dirTanX * ( f - ori.z );
				const Real lensletY = ori.y + dirTanY * ( f - ori.z );

				const Vec2 lensletIndReal = lensletShiftInv + lensletOrientationInv * Vec2(lensletX,lensletY);

				Real lcdPosX;
				Real lcdPosY;

				Vec2 lensletCentralCoord;

				if ( isLensletVertical )
				{
					lensletCentralCoord[0] = std::round( lensletIndReal[0] );
					lensletCentralCoord[1] =             lensletIndReal[1];
				}
				else
				{
					lensletCentralCoord[0] = std::round( lensletIndReal[0] );
					lensletCentralCoord[1] = std::round( lensletIndReal[1] );
				}

				const Vec2 lensletCenter = lensletShift + lensletOrientation * lensletCentralCoord;

				lcdPosX = viewerX + (f+d)/f*( lensletCenter[0] - viewerX );
				lcdPosY = viewerY + (f+d)/f*( lensletCenter[1] - viewerY );

				const Real lcdLambdaX = 0.5 + lcdPosX / lcdSizeX;
				const Real lcdLambdaY = 0.5 - lcdPosY / lcdSizeY;

				const Real lcdPixelX = lcdLambdaX * lcdresX;
				const Real lcdPixelY = lcdLambdaY * lcdresY;

				if ( lcdPixelX < 0 || lcdPixelX > lcdresX ||
					 lcdPixelY < 0 || lcdPixelY > lcdresY )
					continue;

				const Int lcdPixelIntX = std::min<Int>( std::max<Int>( lcdPixelX, 0 ), lcdresX-1 );
				const Int lcdPixelIntY = std::min<Int>( std::max<Int>( lcdPixelY, 0 ), lcdresY-1 );

				Color color = DisplayImage.at<Color>( lcdPixelIntY, lcdPixelIntX );

				const Real w = weightSample * weightRay;
				colorSum += w * color;
				weightsSum += w;
			}
			while ( sampleGen.MoveToNextSample() );
			if ( weightsSum > 0 )
				colorSum = colorSum / weightsSum;
		}
	}

	return true;
}
