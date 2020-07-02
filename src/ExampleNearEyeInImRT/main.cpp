#include <iostream>

#include "LFRayTracerPBRT.h"

#include "DisplayLenslet.h"
#include "DisplayLensletCapture.h"
#include "DisplayLensletShow.h"

#include "Image.h"
#include "ImageAnalysis.h"
#include "SampleAccumCV.h"
#include "SampleGenUniform.h"

#include "RayGenPinhole.h"


using namespace lfrt;


// ToDo: Realistic camera model (eye that can focus).
// ToDo: Metrics consistent with published results.
// ToDo: Test all cases.


const std::string groundtrueImageFilepath = "../groundtrue.exr";
const std::string displayImageFilepath = "../displayimage.exr";
const std::string simulatedImageFilepath = "../simulated.exr";

// Width and height of the images to compare.
const Int width = 512;
const Int height = 512;

// This determines camera FoV.
const Real sceneCornerX = 90;
const Real sceneCornerY = 90;
const Real sceneDistance = 300;

// Display model itself.
DisplayLenslet display;


void RenderGroundTrue( const std::string& scene_filepath )
{

    std::cout << "Ground-true image render started." << std::endl;

    LFRayTracer* raytracer = LFRayTracerPBRTInstance();

    raytracer->LoadScene( scene_filepath );

    std::shared_ptr<const RayGenerator> raygen( new RayGenPinhole( width, height, sceneCornerX, sceneCornerY, sceneDistance ) );

    std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform(3) );

    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );

    raytracer->Render( *raygen, *sampleGen, *sampleAccum );

    cv::Mat result;
    sampleAccumCV->SaveToImage( result );

    std::cout << "Ground-true image render ended." << std::endl;

    cv::imwrite( groundtrueImageFilepath, result );
    cv::namedWindow( "Ground True", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Ground True", result );

}



void RenderDisplayImage( const std::string& scene_filepath )
{
    std::cout << "Display image render started." << std::endl;

    LFRayTracer* raytracer = LFRayTracerPBRTInstance();

    raytracer->LoadScene( scene_filepath );

    const Int width = display.ResolutionLCD[0];
    const Int height = display.ResolutionLCD[1];

    DisplayLensletCapture* displayRaygen = new DisplayLensletCapture( &display );
    std::shared_ptr<const RayGenerator> raygen( displayRaygen );

    std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform(1) );

    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );

    raytracer->Render( *raygen, *sampleGen, *sampleAccum );

    cv::Mat result;
    sampleAccumCV->SaveToImage( result );

    std::cout << "Display image render ended." << std::endl;

    cv::imwrite( displayImageFilepath, result );
    cv::namedWindow( "Display Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display Image", result );
}


void RenderSimulation()
{
    std::cout << "Display simulation started." << std::endl;

    const Int numLensletsX = std::round( display.SizeLCD[0] / display.LensletOrientation()(0,0) );
    const Int numLensletsY = std::round( display.SizeLCD[1] / display.LensletOrientation()(1,1) );

    DisplayLensletShow renderer( &display );
    renderer.LoadScene( displayImageFilepath );
    std::shared_ptr<const RayGenerator> raygen( new RayGenPinhole( width, height, sceneCornerX, sceneCornerY, sceneDistance ) );
    std::shared_ptr<SampleGenerator> sampler( new SampleGenUniform(3) );

    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );

    renderer.Render( *raygen, *sampler, *sampleAccum );

    cv::Mat result;
    sampleAccumCV->SaveToImage( result );

    std::cout << "Display simulation ended." << std::endl;

    cv::imwrite( simulatedImageFilepath, result );
    cv::namedWindow( "Simulation", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Simulation", result );
}



void CompareImages()
{
    cv::Mat groundtrue;
    cv::Mat simulated;

    LoadImageRGB( groundtrueImageFilepath, groundtrue );
    LoadImageRGB( simulatedImageFilepath, simulated );

    const double psnr = ImagePSNR( simulated, groundtrue );
    std::cout << "PSNR: " << psnr << std::endl;

    const cv::Scalar ssim = ImageMSSIM( simulated, groundtrue );
    std::cout << "SSIM: " << ssim[0] << " " << ssim[1] << " " << ssim[2] << std::endl;
}



int main(int argc, char** argv)
{
	std::cout << "Hello world" << std::endl;
    std::cout << "ExampleLenslet" << std::endl;
    std::cout << "Usage: 1 mandatory argument: path to scene model." << std::endl;
    std::cout << std::endl;

    if ( argc != 2 )
    {
        std::cout << "You must provide 1 argument: path to scene model." << std::endl;
        return 1;
    }

	const bool isDisplayLoaded = display.Load( "../../data/lensletdisplay.yaml" );
    if ( !isDisplayLoaded )
    {
        std::cout << "Display model is not loaded." << std::endl;
        return 1;
    }

    RenderGroundTrue( argv[1] );

    RenderDisplayImage( argv[1] );

    RenderSimulation();

    LFRayTRacerPBRTRelease();

    CompareImages();

    cv::waitKey(0);

    return 0;
}