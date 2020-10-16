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
#include "SampleGenDisk.h"

#include "RayGenFocusEye.h"


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

// Eye model parameters.
const Real RetinaToPupilDist = 17;

// Display model itself.
DisplayLenslet display;


struct DisplayRenderCase
{
    std::string Name;
    DisplayLensletCapture::Sampling SamplingType;
    Int NumSecondarySamples;
};

struct PerceivedRenderCase
{
    std::string Name;
    Int NumSecondarySamples;
    Real ObjectInFocusDistance;
};

const std::string output_folder = "output";
//const Int NumPrimarySamples = 5;
//const Int NumSecondarySamples = 8;
const Int NumPrimarySamples = 1;
const Int NumSecondarySamples = 5;


// Provided ray tracer must contain already-loaded scene.
void RenderPerceivedImage( const LFRayTracer* raytracer, cv::Mat& perceived, const PerceivedRenderCase& testCase )
{

    std::cout << "Perceived image render started." << std::endl;
    auto raygenFocus = new RayGenFocusEye();
    raygenFocus->Width  = width;
    raygenFocus->Height = height;
    raygenFocus->RetinaPlaneZ = -RetinaToPupilDist;
    raygenFocus->InFocusPlaneZ = testCase.ObjectInFocusDistance;
    const Real retCoef = -RetinaToPupilDist / sceneDistance;
    raygenFocus->RetinaStart = lfrt::VEC2({ -sceneCornerX*retCoef, -sceneCornerY*retCoef });
    raygenFocus->RetinaEnd   = lfrt::VEC2({  sceneCornerX*retCoef,  sceneCornerY*retCoef });
    std::shared_ptr<const RayGenerator> raygen( raygenFocus );
    std::shared_ptr<SampleGenerator> sampleGen( new SampleGenDisk( NumPrimarySamples, testCase.NumSecondarySamples ) );
    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );
    raytracer->Render( *raygen, *sampleGen, *sampleAccum );
    sampleAccumCV->SaveToImage( perceived );
    std::cout << "Perceived image render ended." << std::endl;
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

    // Define display test cases parameters.
    const std::vector<DisplayRenderCase> rtCases({
        DisplayRenderCase({ "LC", DisplayLensletCapture::Sampling::LensletCenter, 1 }),
        DisplayRenderCase({ "PC", DisplayLensletCapture::Sampling::PupilCenter, 1 }),
        DisplayRenderCase({ "LA", DisplayLensletCapture::Sampling::LensletAverage, 5 })
        }
    );

    // Define peceived test cases parameters.
    const std::vector<PerceivedRenderCase> camCases({
        PerceivedRenderCase({ "Pin", 1, 0 }),
        PerceivedRenderCase({ "Near", NumSecondarySamples, 300 }),
        PerceivedRenderCase({ "Cent", NumSecondarySamples, 400 }),
        PerceivedRenderCase({ "Far", NumSecondarySamples, 600 }),
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

    const Int numCamCases = camCases.size();
    const Int numRtCases = rtCases.size();

    // Render ground true images.
    std::vector<cv::Mat> gtimages( numCamCases );
    for ( Int camCaseInd = 0; camCaseInd < numCamCases; ++camCaseInd )
    {
        const auto& camCase = camCases[camCaseInd];
        cv::Mat& gt_image = gtimages[camCaseInd];
        RenderPerceivedImage( raytracer, gt_image, camCase );
        const std::string filename = output_folder + "/gt_" + camCase.Name + ".exr";
        cv::imwrite( filename, gt_image );
    }

    // Render display images, simulate display, and compare to GT.
    std::vector<cv::Scalar> msevals( numCamCases*numRtCases );
    std::vector<cv::Scalar> psnrvals( numCamCases*numRtCases );
    std::vector<cv::Scalar> msssimvals( numCamCases*numRtCases );
    for ( Int rtCaseInd = 0; rtCaseInd < numRtCases; ++rtCaseInd )
    {
        const auto& rtCase = rtCases[rtCaseInd];
        // Render display image.
        cv::Mat displayimage;
        std::cout << "Display image render started." << std::endl;
        {
            const Int width = display.ResolutionLCD[0];
            const Int height = display.ResolutionLCD[1];
            DisplayLensletCapture* displayRaygen = new DisplayLensletCapture( &display, rtCase.SamplingType );
            std::shared_ptr<const RayGenerator> raygen( displayRaygen );
            std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform( NumPrimarySamples, rtCase.NumSecondarySamples ) );
            SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
            std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );
            raytracer->Render( *raygen, *sampleGen, *sampleAccum );
            sampleAccumCV->SaveToImage( displayimage );
            const std::string filename = output_folder + "/display_" + rtCase.Name + ".exr";
            cv::imwrite( filename, displayimage );
        }
        std::cout << "Display image render ended." << std::endl;
        // Create display simulation.
        DisplayLensletShow renderer( &display );
        renderer.DisplayImage = displayimage;
        // Render display simulation and compare to GT.
        for ( Int camCaseInd = 0; camCaseInd < numCamCases; ++camCaseInd )
        {
            const auto& camCase = camCases[camCaseInd];
            cv::Mat perceived;
            RenderPerceivedImage( &renderer, perceived, camCase );
            const std::string filename = output_folder + "/sim_" + rtCase.Name + "_" + camCase.Name + ".exr";
            cv::imwrite( filename, perceived );
            const auto& gtimage = gtimages[camCaseInd];
            const Int statInd = rtCaseInd*numCamCases + camCaseInd;
            msevals[statInd] = ImageValueMSE( perceived, gtimage );
            psnrvals[statInd] = ImageValuePSNR( perceived, gtimage );
            msssimvals[statInd] = ImageValueMSSIM( perceived, gtimage );
        }
    }

    // Save statistics to file.
    std::fstream msefile( output_folder + "/mse.txt", std::fstream::out );
    std::fstream psnrfile( output_folder + "/psnr.txt", std::fstream::out );
    std::fstream msssimfile( output_folder + "/msssim.txt", std::fstream::out );
    for ( Int rtCaseInd = 0; rtCaseInd < numRtCases; ++rtCaseInd )
    {
        const auto& rtCase = rtCases[rtCaseInd];
        for ( Int camCaseInd = 0; camCaseInd < numCamCases; ++camCaseInd )
        {
            const auto& camCase = camCases[camCaseInd];
            const Int statInd = rtCaseInd*numCamCases + camCaseInd;
            const auto& mse = msevals[statInd];
            const auto& psnr = psnrvals[statInd];
            const auto& msssim = msssimvals[statInd];
            msefile << (rtCase.Name+"_"+camCase.Name) << " " << mse[0] << " " << mse[1] << " " << mse[2] << std::endl;
            psnrfile << (rtCase.Name+"_"+camCase.Name) << " " << psnr[0] << " " << psnr[1] << " " << psnr[2] << std::endl;
            msssimfile << (rtCase.Name+"_"+camCase.Name) << " " << msssim[0] << " " << msssim[1] << " " << msssim[2] << std::endl;
        }
    }
    msefile.close();
    psnrfile.close();
    msssimfile.close();

    LFRayTRacerPBRTRelease();

    return 0;
}