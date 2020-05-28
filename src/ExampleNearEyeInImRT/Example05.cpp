#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

#include "BaseTypes.h"
#include "RayTracer.h"
#include "CameraRealistic.h"
#include "Image2D.h"
#include "LFRendererRoutines.h"

#include "SceneRectangles.h"

#include "ImageAnalysis.h"


#include "LFDisplay05.h"




// Near-eye Integral Imaging display:
//    * elemental image is rectangular;
//    * each microlens is a thin spherical lens;
//    * microlens array not necessary aligned with EI array;
//    * no color aberration;
//    * custom subpixel layout;
//    * no image layout, everything is stored in one 2D image;
//    * back-light is uniform in all directions;
//    * lenslet array is planar with plane z=0.



using namespace LFOptics;

using ss = std::stringstream;

struct SystemSetup
{
	Real pupilDiameter = 5.0;
	Real lensletFocalLens = 50.0;

	Real distanceFromPupilToLenslet = 20.0;
	//Real distanceFromPupilToSceneCenter = 1000;
	Real distanceFromPupilToSceneCenter = 400;
	Real distanceFromPupilToRetina = 17.0;
	Real distanceFromLensletToLCD = 4.0;

	Real distanceFromPupilToFocusedObject = 400;

	Real pinhole_tanHalfFov = 0.3;

	Vec2i retinaRes = 512 * Vec2i(1,1);
	Vec2i displayRes = 512 * Vec2i(1,1);

	Vec2 lensletSize = 1.2 * Vec2(1,1);
	Vec2 patchSize = 1.2 * Vec2(1,1);
	Vec2 retinaSize = 10.0 * Vec2(1,1);
	Vec2 screenSize = 10.0 * Vec2(1,1);
};


struct SimulationSetup
{
	bool isUseBarriers = false;
	PixelInterpolation pixelInterpolation = PixelInterpolation::Linear;
	Int numSamplesInPixelRow = 5;
	Int numSamplesOnPupilRow = 8;
	Int numThreads = 32;
	Int numImagesInSequence = 1;
	Real cdpStart = 200;
	Real cdpEnd = 600;
};


SystemSetup SystemSetupByTanFoV()
{
	SystemSetup setup;

	setup.pinhole_tanHalfFov = 0.3;
	const Real lensletDiameter = 1.2;
	const Real displayPixelSize = 0.01; // 10 micrometers.
	//const Real retinaPixelSize = 0.0025; // 2.5 micrometers.
	//const Real displayPixelSize = 0.1; // 100 micrometers.
	const Real retinaPixelSize = 0.025; // 25 micrometers.

	const Real retinaDiameter = 2.0 * setup.distanceFromPupilToRetina * setup.pinhole_tanHalfFov;
	//const Real screenDiameter = 2.0 * setup.distanceFromPupilToLenslet * setup.pinhole_tanHalfFov;
	//const Real screenDiameter = 2.0 * (setup.distanceFromPupilToLenslet + setup.distanceFromLensletToLCD) * setup.pinhole_tanHalfFov;
	const Real screenDiameter = 2.0 * (setup.distanceFromPupilToLenslet + setup.distanceFromLensletToLCD) * (1.2*setup.pinhole_tanHalfFov);
	setup.retinaSize = retinaDiameter * Vec2(1,1);
	setup.screenSize = screenDiameter * Vec2(1,1);

	const Real patchDiameter = lensletDiameter;
	setup.lensletSize = lensletDiameter * Vec2(1,1);
	setup.patchSize = patchDiameter * Vec2(1,1);

	setup.distanceFromLensletToLCD = 0.5 * lensletDiameter / setup.pinhole_tanHalfFov;
	const Real distanceFromLensletToSceneCenter = setup.distanceFromPupilToSceneCenter - setup.distanceFromPupilToLenslet;
	//setup.lensletFocalLens = 1.0 / ( 1.0/setup.distanceFromLensletToLCD + 1.0/distanceFromLensletToSceneCenter );
	setup.lensletFocalLens = 1.0 / ( 1.0/setup.distanceFromLensletToLCD - 1.0/distanceFromLensletToSceneCenter );
	setup.retinaRes = std::lround(retinaDiameter/ retinaPixelSize) * Vec2i(1,1);
	setup.displayRes = std::lround(screenDiameter/displayPixelSize) * Vec2i(1,1);

	return setup;
}


SystemSetup SystemSetupByPupilCoverage()
{
	SystemSetup setup;

	const Real lensletDiameter = 1.2;
	const Real displayPixelSize = 0.01; // 10 micrometers.
	//const Real retinaPixelSize = 0.0025; // 2.5 micrometers.
	//const Real displayPixelSize = 0.1; // 100 micrometers.
	const Real retinaPixelSize = 0.025; // 25 micrometers.

	const Real retinaDiameter = 2.0 * setup.distanceFromPupilToRetina * setup.pinhole_tanHalfFov;
	//const Real screenDiameter = 2.0 * setup.distanceFromPupilToLenslet * setup.pinhole_tanHalfFov;
	//const Real screenDiameter = 2.0 * (setup.distanceFromPupilToLenslet + setup.distanceFromLensletToLCD) * setup.pinhole_tanHalfFov;
	const Real screenDiameter = 2.0 * (setup.distanceFromPupilToLenslet + setup.distanceFromLensletToLCD) * (1.5*setup.pinhole_tanHalfFov);
	setup.retinaSize = retinaDiameter * Vec2(1,1);
	setup.screenSize = screenDiameter * Vec2(1,1);

	//const Real patchDiameter = lensletDiameter;
	setup.lensletSize = lensletDiameter * Vec2(1,1);

	setup.distanceFromLensletToLCD = setup.distanceFromPupilToLenslet * lensletDiameter / (setup.pupilDiameter - lensletDiameter);
	const Real patchDiameter = setup.pupilDiameter * setup.distanceFromLensletToLCD / setup.distanceFromPupilToLenslet;
	setup.patchSize = patchDiameter * Vec2(1,1);

	const Real distanceFromLensletToSceneCenter = setup.distanceFromPupilToSceneCenter - setup.distanceFromPupilToLenslet;
	//setup.lensletFocalLens = 1.0 / ( 1.0/setup.distanceFromLensletToLCD + 1.0/distanceFromLensletToSceneCenter );
	setup.lensletFocalLens = 1.0 / ( 1.0/setup.distanceFromLensletToLCD - 1.0/distanceFromLensletToSceneCenter );
	setup.retinaRes = std::lround(retinaDiameter/ retinaPixelSize) * Vec2i(1,1);
	setup.displayRes = std::lround(screenDiameter/displayPixelSize) * Vec2i(1,1);

	return setup;
}


const SystemSetup curSystemSetup = SystemSetupByPupilCoverage();
const SimulationSetup curSimulationSetup;



CameraRealistic SetupCamera()
{
	CameraRealistic camera;
	camera.ImagePlaneZ() = -curSystemSetup.distanceFromPupilToRetina;
	camera.ImagePlaneRange().min = -0.5 * curSystemSetup.retinaSize;
	camera.ImagePlaneRange().max = 0.5 * curSystemSetup.retinaSize;
	camera.ImageResolution() = curSystemSetup.retinaRes;
	camera.ObjectiveInvFocalLength() = 1.0 / curSystemSetup.distanceFromPupilToRetina + 1.0 / curSystemSetup.distanceFromPupilToFocusedObject;
	camera.SetNumSamplesInApertureRow(curSimulationSetup.numSamplesOnPupilRow);
	camera.SetNumSamplesInPixelRow(curSimulationSetup.numSamplesInPixelRow);
	camera.SetApertureShape(ApertureShape::Disk);
	camera.SetApertureDiameter(curSystemSetup.pupilDiameter);
	camera.SetCameraPositioning(Affine3(Mat33(
		-1, 0, 0,
		0, 1, 0,
		0, 0, -1
	), Vec3(0, 0, curSystemSetup.distanceFromPupilToSceneCenter)));
	return camera;
}


LFDisplay05 GenerateDisplayModel()
{
	LFDisplay05 display;
	display.ColorPanel().PanelSize().min = -0.5*curSystemSetup.screenSize;
	display.ColorPanel().PanelSize().max =  0.5*curSystemSetup.screenSize;
	display.ColorPanel().SetLensletToSurfaceByLensletSize(curSystemSetup.lensletSize, Vec2(0, 0));
	display.ColorPanel().SetPatchToSurfaceByPatchSize(curSystemSetup.patchSize, Vec2(0, 0));
	display.ColorPanel().ImagePlaneZ() = -curSystemSetup.distanceFromLensletToLCD;
	display.ColorPanel().InvLensletFocusDistance() = 1.0/ curSystemSetup.lensletFocalLens;
	display.IsUseBarriers() = curSimulationSetup.isUseBarriers;
	display.GetPixelInterpolation() = curSimulationSetup.pixelInterpolation;
	display.UpdateImageLayout(curSystemSetup.displayRes);
	display.Positioning() = Affine3(Mat33(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	), Vec3(0, 0, curSystemSetup.distanceFromPupilToSceneCenter - curSystemSetup.distanceFromPupilToLenslet));
	//display.SaveToJson(displayjsonpath);
	return display;
}


const Mat44 colortransform = Mat44(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0.299, 0.587, 0.114, 0
);

const Int num_hist_bins = 20;

std::vector<std::string> statistics_labels;
std::vector<Vec4> statistics_MSE;
std::vector<Vec4> statistics_PSNR;
std::vector< std::vector<Vec4> > statistics_histograms;
std::vector<Vec4> statistics_SSIM;


void EvaluateStatistics( const Image2D& image, const Image2D& gt, const std::string& label, Image2D& image_tmp )
{
	const Vec4 mse = ImageAnalysis::ImageMSE(image, gt, colortransform);
	const Vec4 psnr = -10.0 * Vec4(std::log10(mse.x), std::log10(mse.y), std::log10(mse.z), std::log10(mse.w));
	statistics_labels.push_back(label);
	statistics_MSE.push_back(mse);
	statistics_PSNR.push_back(psnr);

	std::vector<Vec4> hist;
	Image2D& diff = image_tmp;
	ImageAnalysis::ImageDiff(image, gt, diff, true);
	ImageAnalysis::ImageHistogram( diff, Vec4(0,0,0,0), Vec4(1,1,1,1), num_hist_bins, hist, true, colortransform );
	statistics_histograms.push_back(hist);

	Image2D& ssim = image_tmp;
	const Vec4 ssim_val = ImageAnalysis::ImageSSIM(image, gt, ssim, colortransform);
	statistics_SSIM.push_back(ssim_val);
	ssim.Save("ssim/ssim-"+label+".exr");
	ssim.Save("toshow/ssim-"+label+".png");
}



int main(int argc, char** argv)
{
	std::cout << "Hello world" << std::endl;

	LFRendererRoutines::NumberOfThreads() = curSimulationSetup.numThreads;

#if 0
	SceneSimpleBalls scene(Affine3( Mat33::diagonalize(Vec3(1,1,1)), Vec3(0,0,0) ));
#else
	SceneRectangles scene;
	{
		SceneRectangles::VectorGraphicsPlane usaf1951 = SceneRectangles::LoadFromFile("../../data/USAF-1951-rectangles.txt");
		scene.planes.push_back(usaf1951);
		scene.planes.push_back(usaf1951);
		scene.planes.push_back(usaf1951);
		scene.planes[0].xydim = MinMax2Real( -90, -90, 0, 0 );
		scene.planes[1].xydim = MinMax2Real( -60, -60, 60, 60 );
		scene.planes[2].xydim = MinMax2Real( 0, 0, 180, 180 );
		scene.planes[0].z = 100;
		scene.planes[1].z = 0;
		scene.planes[2].z = -200;
	}
#endif

	Image2D pinhole_gt(curSystemSetup.retinaRes.x, curSystemSetup.retinaRes.y);
	Image2D realistic_gt(curSystemSetup.retinaRes.x, curSystemSetup.retinaRes.y);
	Image2D pinhole_perceived(curSystemSetup.retinaRes.x, curSystemSetup.retinaRes.y);
	Image2D realistic_perceived(curSystemSetup.retinaRes.x, curSystemSetup.retinaRes.y);
	Image2D diff(curSystemSetup.retinaRes.x, curSystemSetup.retinaRes.y);
	Image2D displayImage(curSystemSetup.displayRes.x, curSystemSetup.displayRes.y);

	RayTracer raytracer(&scene);
	LFDisplay05 display = GenerateDisplayModel();
	LFDisplay05::DisplaySimulation displaySimulation = display.CreateDisplaySimulation(displayImage);

	CameraRealistic cameraRealistic = SetupCamera();
	CameraRealistic cameraPinhole = cameraRealistic;
	cameraPinhole.SetNumSamplesInApertureRow(1);

	const bool isGenerateGT = true;
	const bool isGenerateLensletCenter = true;
	const bool isGeneratePupilCenter = true;
	const bool isGenerateLensletAverage = true;

	const bool isGeneratePinhole = true;
	const bool isGenerateRealistic = true;
	const bool isCalculateDiff = true;
	const bool isCalculateStatistics = true;


	std::system( "mkdir displayimage" );
	std::system( "mkdir groundtrue" );
	std::system( "mkdir perceived" );
	std::system( "mkdir raytable" );
	std::system( "mkdir toshow" );
	std::system( "mkdir diff" );
	std::system( "mkdir ssim" );


	if ( isGenerateGT )
	{
		if ( isGeneratePinhole )
		{
			Image2D& image = pinhole_gt;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraPinhole, raytracer, image.ImageRange(), true);
			image.Save("groundtrue/pinhole-gt.exr");
			image.Save("toshow/pinhole-gt.png");
		}
		
		if ( isGenerateRealistic )
		{
			Image2D& image = realistic_gt;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraRealistic, raytracer, image.ImageRange(), true);
			image.Save("groundtrue/realistic-gt.exr");
			image.Save("toshow/realistic-gt.png");
		}
	}


	if ( isGenerateLensletCenter )
	{
		const Vec3 eyePos = Vec3( 0, 0, curSystemSetup.distanceFromPupilToLenslet );
		LFDisplay05::RayGeneration raygen = display.CreateRayGeneration( eyePos, LFDisplay05::Sampling::LensletCenter );
		const Affine2& imageToSurface = display.GetImageLayout().ImageToSurface(0);
		LFRendererRoutines::RaytraceMultithreaded(displayImage, raygen, raytracer, displayImage.ImageRange(), curSimulationSetup.numSamplesInPixelRow, imageToSurface);
		displayImage.Save("displayimage/displayimage-lensletcenter.exr");

		if ( isGeneratePinhole )
		{
			Image2D& image = pinhole_perceived;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraPinhole, displaySimulation, image.ImageRange(), true);
			image.Save("perceived/pinhole-lensletcenter.exr");
			image.Save("toshow/pinhole-lensletcenter.png");

			if ( isCalculateDiff )
			{
				ImageAnalysis::ImageDiff(image, pinhole_gt, diff, true);
				diff.Save("diff/diff-pinhole-lensletcenter.exr");
				diff.Save("toshow/diff-pinhole-lensletcenter.png");
			}
		}

		if ( isGenerateRealistic )
		{
			Image2D& image = realistic_perceived;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraRealistic, displaySimulation, image.ImageRange(), true);
			image.Save("perceived/realistic-lensletcenter.exr");
			image.Save("toshow/realistic-lensletcenter.png");

			if ( isCalculateDiff )
			{
				ImageAnalysis::ImageDiff(image, realistic_gt, diff, true);
				diff.Save("diff/diff-realistic-lensletcenter.exr");
				diff.Save("toshow/diff-realistic-lensletcenter.png");
			}
		}
	}


	if ( isGeneratePupilCenter ) 
	{
		const Vec3 eyePos = Vec3( 0, 0, curSystemSetup.distanceFromPupilToLenslet );
		LFDisplay05::RayGeneration raygen = display.CreateRayGeneration( eyePos, LFDisplay05::Sampling::PupilCenter );
		const Affine2& imageToSurface = display.GetImageLayout().ImageToSurface(0);
		LFRendererRoutines::RaytraceMultithreaded(displayImage, raygen, raytracer, displayImage.ImageRange(), curSimulationSetup.numSamplesInPixelRow, imageToSurface);
		displayImage.Save("displayimage/displayimage-pupilcenter.exr");

		if ( isGeneratePinhole )
		{
			Image2D& image = pinhole_perceived;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraPinhole, displaySimulation, image.ImageRange(), true);
			image.Save("perceived/pinhole-pupilcenter.exr");
			image.Save("toshow/pinhole-pupilcenter.png");

			if ( isCalculateDiff )
			{
				ImageAnalysis::ImageDiff(image, pinhole_gt, diff, true);
				diff.Save("diff/diff-pinhole-pupilcenter.exr");
				diff.Save("toshow/diff-pinhole-pupilcenter.png");
			}
		}

		if ( isGenerateRealistic )
		{
			Image2D& image = realistic_perceived;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraRealistic, displaySimulation, image.ImageRange(), true);
			image.Save("perceived/realistic-pupilcenter.exr");
			image.Save("toshow/realistic-pupilcenter.png");

			if ( isCalculateDiff )
			{
				ImageAnalysis::ImageDiff(image, realistic_gt, diff, true);
				diff.Save("diff/diff-realistic-pupilcenter.exr");
				diff.Save("toshow/diff-realistic-pupilcenter.png");
			}
		}
	}


	if ( isGenerateLensletAverage )
	{
		const Vec3 eyePos = Vec3( 0, 0, curSystemSetup.distanceFromPupilToLenslet );
		LFDisplay05::RayGeneration raygen = display.CreateRayGeneration( eyePos, LFDisplay05::Sampling::LensletAverage );
		const Affine2& imageToSurface = display.GetImageLayout().ImageToSurface(0);
		LFRendererRoutines::RaytraceMultithreaded(displayImage, raygen, raytracer, displayImage.ImageRange(), curSimulationSetup.numSamplesInPixelRow, imageToSurface);
		displayImage.Save("displayimage/displayimage-lensletaverage.exr");

		if ( isGeneratePinhole )
		{
			Image2D& image = pinhole_perceived;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraPinhole, displaySimulation, image.ImageRange(), true);
			image.Save("perceived/pinhole-lensletaverage.exr");
			image.Save("toshow/pinhole-lensletaverage.png");

			if ( isCalculateDiff )
			{
				ImageAnalysis::ImageDiff(image, pinhole_gt, diff, true);
				diff.Save("diff/diff-pinhole-lensletaverage.exr");
				diff.Save("toshow/diff-pinhole-lensletaverage.png");
			}
		}

		if ( isGenerateRealistic )
		{
			Image2D& image = realistic_perceived;
			LFRendererRoutines::RaytraceMultithreaded(image, cameraRealistic, displaySimulation, image.ImageRange(), true);
			image.Save("perceived/realistic-lensletaverage.exr");
			image.Save("toshow/realistic-lensletaverage.png");

			if ( isCalculateDiff )
			{
				ImageAnalysis::ImageDiff(image, realistic_gt, diff, true);
				diff.Save("diff/diff-realistic-lensletaverage.exr");
				diff.Save("toshow/diff-realistic-lensletaverage.png");
			}
		}
	}


	if (isCalculateStatistics)
	{
		{
			pinhole_gt.Load(  "groundtrue/pinhole-gt.exr");
			realistic_gt.Load("groundtrue/realistic-gt.exr");
		}
		{
			Image2D image_tmp;

			if ( pinhole_perceived.Load("perceived/pinhole-lensletcenter.exr") )
				EvaluateStatistics(pinhole_perceived, pinhole_gt, "pinhole-lensletcenter", image_tmp);
			if ( realistic_perceived.Load("perceived/realistic-lensletcenter.exr") )
				EvaluateStatistics(realistic_perceived, realistic_gt, "realistic-lensletcenter", image_tmp);

			if ( pinhole_perceived.Load("perceived/pinhole-pupilcenter.exr") )
				EvaluateStatistics(pinhole_perceived, pinhole_gt, "pinhole-pupilcenter", image_tmp);
			if ( realistic_perceived.Load("perceived/realistic-pupilcenter.exr") )
				EvaluateStatistics(realistic_perceived, realistic_gt, "realistic-pupilcenter", image_tmp);

			if ( pinhole_perceived.Load("perceived/pinhole-lensletaverage.exr") )
				EvaluateStatistics(pinhole_perceived, pinhole_gt, "pinhole-lensletaverage", image_tmp);
			if ( realistic_perceived.Load("perceived/realistic-lensletaverage.exr") )
				EvaluateStatistics(realistic_perceived, realistic_gt, "realistic-lensletaverage", image_tmp);
		}
		{
			std::fstream filestream_mse;
			std::fstream filestream_psnr;
			std::fstream filestream_hist;
			std::fstream filestream_ssim;
			try
			{
				filestream_mse.open(  "mse.txt", std::ofstream::out );
				filestream_psnr.open( "psnr.txt", std::ofstream::out );
				filestream_hist.open( "histograms.txt", std::ofstream::out );
				filestream_ssim.open( "ssim.txt", std::ofstream::out );
				filestream_mse << std::setw(4);
				for ( Int i = 0; i < statistics_labels.size(); ++i )
				{
					const std::string& label = statistics_labels[i];
					const Vec4& mse = statistics_MSE[i];
					const Vec4& psnr = statistics_PSNR[i];
					const Vec4& ssim = statistics_SSIM[i];
					filestream_mse << label << " " << mse.x << " " << mse.y << " " << mse.z << " " << mse.w << std::endl;
					filestream_psnr << label << " " << psnr.x << " " << psnr.y << " " << psnr.z << " " << psnr.w << std::endl;
					filestream_ssim << label << " " << ssim.x << " " << ssim.y << " " << ssim.z << " " << ssim.w << std::endl;
					filestream_hist << label;
					for ( Int binInd = 0; binInd < num_hist_bins; ++binInd )
					{
						filestream_hist << " " << statistics_histograms[i][binInd].w;
					}
					filestream_hist << std::endl;
				}
			}
			catch ( ... )
			{
			}
			filestream_mse.close();
			filestream_psnr.close();
			filestream_hist.close();
			filestream_ssim.close();
		}
	}

}