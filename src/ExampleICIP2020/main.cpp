#include <iostream>
#include <filesystem>
#include <fstream>

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


const std::string groundtrueImageFilepath = "output/groundtrue.exr";
const std::string displayImageFilepath = "output/displayimage.exr";
const std::string simulatedImageFilepath = "output/simulated.exr";

// Width and height of the images to compare.
const Int width = 512;
const Int height = 512;

// This determines camera FoV.
const Real sceneCornerX = 90;
const Real sceneCornerY = 90;
const Real sceneDistance = 300;

// Display model itself.
DisplayLenslet display;


struct TestCase
{
    std::string Name;
    DisplayLensletCapture::Sampling SamplingType;
    Int NumSamples;
};

const std::string output_folder = "output";
const Int NumPerceivedSamples = 5;


// Provided ray tracer must contain already-loaded scene.
void RenderGroundTruth( const LFRayTracer* raytracer, cv::Mat& gtimage )
{

    std::cout << "Ground-true image render started." << std::endl;
    std::shared_ptr<const RayGenerator> raygen( new RayGenPinhole( width, height, sceneCornerX, sceneCornerY, sceneDistance ) );
    std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform( NumPerceivedSamples ) );
    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );
    raytracer->Render( *raygen, *sampleGen, *sampleAccum );
    sampleAccumCV->SaveToImage( gtimage );
    std::cout << "Ground-true image render ended." << std::endl;
}


// Provided ray tracer must contain already-loaded scene.
void RunTestCase( const TestCase& testCase, const LFRayTracer* raytracer, const cv::Mat& gtimage )
{
    // Define case-related images images.
    cv::Mat displayimage;
    cv::Mat perceivedimage;

    // Create case-related folder.
    const std::string casedir = output_folder + "/" + testCase.Name;
    if ( !std::filesystem::exists( casedir ) )
    {
        if ( !std::filesystem::create_directory( casedir ) )
        {
            std::cout << "Error: Cannot create directory: " << casedir << std::endl;
            return;
        }
    }

    // Render display image.
    std::cout << "Display image render started." << std::endl;
    {
        const Int width = display.ResolutionLCD[0];
        const Int height = display.ResolutionLCD[1];
        DisplayLensletCapture* displayRaygen = new DisplayLensletCapture( &display, testCase.SamplingType );
        std::shared_ptr<const RayGenerator> raygen( displayRaygen );
        std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform(testCase.NumSamples) );
        SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
        std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );
        raytracer->Render( *raygen, *sampleGen, *sampleAccum );
        sampleAccumCV->SaveToImage( displayimage );
    }
    std::cout << "Display image render ended." << std::endl;
    cv::imwrite( casedir+"/displayimage.exr", displayimage );

    // Render perceived image.
    std::cout << "Display simulation started." << std::endl;
    {
        const Int numLensletsX = std::round( display.SizeLCD[0] / display.LensletOrientation()(0,0) );
        const Int numLensletsY = std::round( display.SizeLCD[1] / display.LensletOrientation()(1,1) );
        DisplayLensletShow renderer( &display );
        renderer.DisplayImage = displayimage;
        std::shared_ptr<const RayGenerator> raygen( new RayGenPinhole( width, height, sceneCornerX, sceneCornerY, sceneDistance ) );
        std::shared_ptr<SampleGenerator> sampler( new SampleGenUniform( NumPerceivedSamples ) );
        SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
        std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );
        renderer.Render( *raygen, *sampler, *sampleAccum );
        sampleAccumCV->SaveToImage( perceivedimage );
    }
    std::cout << "Display simulation ended." << std::endl;
    cv::imwrite( casedir+"/simulated.exr", perceivedimage );

    // Compare perceived image vs ground truth.
    const cv::Scalar psnr = ImageValuePSNR( perceivedimage, gtimage );
    std::cout << "PSNR: " << psnr[0] << " " << psnr[1] << " " << psnr[2] << std::endl;
    const cv::Scalar ssim = ImageValueMSSIM( perceivedimage, gtimage );
    std::cout << "SSIM: " << ssim[0] << " " << ssim[1] << " " << ssim[2] << std::endl;
    // Write statistics to file.
    std::fstream file( casedir+"/statistics.txt" , std::fstream::out );
    if ( !file.is_open() )
    {
        std::cout << "Error: Cannot create text file to write statistics." << std::endl;
        return;
    }
    file << "PSNR: " << psnr[0] << " " << psnr[1] << " " << psnr[2] << std::endl;
    file << "MS-SSIM: " << ssim[0] << " " << ssim[1] << " " << ssim[2] << std::endl;
    file.close();
}


int main(int argc, char** argv)
{
    std::cout << "Example ICIP-2020" << std::endl;
    std::cout << "List of needed arguments:" << std::endl;
    std::cout << "1) Path to pbrt-v3 scene model." << std::endl;
    std::cout << "2) Path to lenslet-based display model." << std::endl;
    std::cout << "Once executed, program will create foder 'output' and fill it." << std::endl;
    std::cout << std::endl;

    if ( argc != 3 )
    {
        std::cout << "Error: Number of provided arguments is not correct." << std::endl;
        return 1;
    }

    const std::string scenepath = argv[1];
    const std::string displaypath = argv[2];

	const bool isDisplayLoaded = display.Load( displaypath );
    if ( !isDisplayLoaded )
    {
        std::cout << "Display model is not loaded." << std::endl;
        return 1;
    }

    if ( !std::filesystem::exists( output_folder ) )
    {
        if ( !std::filesystem::create_directory( output_folder ) )
        {
            std::cout << "Error: Cannot create folder 'output'." << std::endl;
            return 1;
        }
    }

    // Define test cases parameters.
    const std::vector<TestCase> testCases({
        TestCase({ "LC", DisplayLensletCapture::Sampling::LensletCenter, 1 }),
        TestCase({ "PC", DisplayLensletCapture::Sampling::PupilCenter, 1 }),
        TestCase({ "LA", DisplayLensletCapture::Sampling::LensletAverage, 5 })
        }
    );

    LFRayTracer* raytracer = LFRayTracerPBRTInstance();
    if ( raytracer == nullptr )
    {
        std::cout << "Error: Cannot initialize ray tracer." << std::endl;
        return 1;
    }
    if ( !raytracer->LoadScene( scenepath ) )
    {
        std::cout << "Error: Cannot load scene." << std::endl;
        return 1;
    }

    cv::Mat gt_image;
    RenderGroundTruth( raytracer, gt_image );
    cv::imwrite( output_folder+"/gt.exr", gt_image );

    for ( auto caseiter = testCases.begin(); caseiter != testCases.end(); ++caseiter )
    {
        RunTestCase( *caseiter, raytracer, gt_image );
    }

    LFRayTRacerPBRTRelease();

    return 0;
}