#include <iostream>

#include "LFRayTracerPBRT.h"

#include "DisplayLenslet.h"
#include "DisplayLensletCapture.h"
#include "DisplayLensletShow.h"

#include "SampleAccumCV.h"
#include "SampleGenUniform.h"


using namespace lfrt;


const std::string groundtrueImageFilepath = "../groundtrue.exr";
const std::string displayImageFilepath = "../displayimage.exr";
const std::string simulatedImageFilepath = "../simulated.exr";


const Int width = 512;
const Int height = 512;


void RenderGroundTrue( const std::string& scene_filepath, const Int width, const Int height )
{

    std::cout << "Ground-true image render started." << std::endl;

    LFRayTracer* raytracer = LFRayTracerPBRTInstance();

    raytracer->LoadScene( scene_filepath );

    //DisplayLensletCapture* displayRaygen = new DisplayLensletCapture( &display );
    //std::shared_ptr<const RayGenerator> raygen( displayRaygen );
    std::shared_ptr<const RayGenerator> raygen( raytracer->CreateDefaultRayGenerator( width, height ) );

    std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform(3) );

    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );

    raytracer->Render( *raygen, *sampleGen, *sampleAccum );

    cv::Mat result;
    sampleAccumCV->SaveToImage( result );

    //LFRayTRacerPBRTRelease();

    std::cout << "Ground-true image render ended." << std::endl;

    cv::imwrite( groundtrueImageFilepath, result );
    cv::namedWindow( "Ground True", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Ground True", result );

}



void RenderDisplayImage( const std::string& scene_filepath, const DisplayLenslet& display )
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

    //LFRayTRacerPBRTRelease();

    std::cout << "Display image render ended." << std::endl;

    cv::imwrite( displayImageFilepath, result );
    cv::namedWindow( "Display Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display Image", result );
}



void RenderSimulation( const DisplayLenslet& display )
{
    std::cout << "Display simulation started." << std::endl;

    const Int numLensletsX = std::round( display.SizeLCD[0] / display.LensletOrientation()(0,0) );
    const Int numLensletsY = std::round( display.SizeLCD[1] / display.LensletOrientation()(1,1) );

    DisplayLensletShow renderer( &display );
    renderer.LoadScene( displayImageFilepath );
    std::shared_ptr<const RayGenerator> raygen( renderer.CreateDefaultRayGenerator( width, height ) );
    //std::shared_ptr<SampleGenerator> sampler( renderer.CreateDefaultSampleGenerator( width, height ) );
    std::shared_ptr<SampleGenerator> sampler( new SampleGenUniform(5) );
    //SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    //std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );
    cv::Mat result;
    //renderer.Render( *raygen, *sampler, *sampleAccum );
    renderer.Render( *raygen, *sampler, result, width, height );
    //sampleAccumCV->SaveToImage( result );

    std::cout << "Display simulation ended." << std::endl;

    cv::imwrite( simulatedImageFilepath, result );
    cv::namedWindow( "Simulation", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Simulation", result );
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

	DisplayLenslet display;
	const bool isDisplayLoaded = display.Load( "../../data/lensletdisplay.yaml" );
    if ( !isDisplayLoaded )
    {
        std::cout << "Display model is not loaded." << std::endl;
        return 1;
    }

    RenderGroundTrue( argv[1], width, height );

    RenderDisplayImage( argv[1], display );

    RenderSimulation( display );

    LFRayTRacerPBRTRelease();

    cv::waitKey(0);

    return 0;
}