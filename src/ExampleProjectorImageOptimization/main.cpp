#include <iostream>

#include <cstdlib>

#include "LFRayTracerPBRT.h"

#include "Image.h"
#include "SampleAccumCV.h"
#include "SampleGenUniform.h"

#include "DisplayProjectorAligned.h"
#include "DisplayProjectorsCapture.h"
#include "DisplayProjectorsOptimization.h"
#include "DisplayProjectorsShow.h"

#include "RayGenPinhole.h"
#include "ObserverSpace.h"


DisplayProjectorAligned display;
std::vector<Vec3> projectorPositions;
Int width = 0;
Int height = 0;


const ObserverSpace observerSpace( { Vec3(-500,0,0), Vec3(10,0,0), 101 } );

const Int numIterations = 100;


using namespace lfrt;
using ss = std::stringstream;




int main( int argc, char** argv )
{
    std::cout << "Hello world" << std::endl;
    std::cout << "ExampleProjectorImageOptimization" << std::endl;
    std::cout << "Usage: 1 mandatory argument: path to scene model." << std::endl;
    std::cout << std::endl;

    if ( argc != 2 )
    {
        std::cout << "You must provide 1 argument: path to scene model." << std::endl;
        return 1;
    }

    std::cout << "Scene model: " << argv[1] << std::endl;

    if ( !display.Load( "../../data/display-projectors-aligned.yaml" ) )
    {
        std::cout << "Cannot load display model." << std::endl;
        return 1;
    }
    width  = display.ProjectorResolution[0];
    height = display.ProjectorResolution[1];
    display.FillProjectorsPositions( projectorPositions );
    const Int numProjectors = projectorPositions.size();
    const Int numViewerPositions = observerSpace.NumPositions();


    std::cout << std::endl;
    std::cout << "Initializing basic parameters complete." << std::endl;
    std::cout << "What would you like to do?" << std::endl;
    std::cout << "1 - generate ground-true images" << std::endl;
    std::cout << "2 - generate projector images" << std::endl;
    std::cout << "3 - generate perceived images (requires step 2)" << std::endl;
    std::cout << "4 - generate iterative projector-perceived images (requires steps 1 and 2)" << std::endl;

    Int choice = -1;
    std::cin >> choice;

    // Rendering helper classes.
    std::shared_ptr<const RayGenerator> raygen = nullptr;
    std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform(3) );
    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );
    cv::Mat result;

    // Display data.
    const Real halfSizeX = display.HalfPhysSize[0];
    const Real halfSizeY = display.HalfPhysSize[1];

    // Process the choice.
    switch ( choice )
    {
    case 1: {
        std::system( "mkdir GroundTrueImages" );
        LFRayTracer* raytracer = LFRayTracerPBRTInstance();
        raytracer->LoadScene( argv[1] );
        for ( Int viewInd = 0; viewInd < numViewerPositions; ++viewInd )
        {
            const Vec3 pos = observerSpace.Position( viewInd );
            const std::string image_filepath = (ss() << "GroundTrueImages" << "/" << std::setfill('0') << std::setw(4) << viewInd << ".exr").str();
            raygen.reset( new RayGenPinhole( width, height, -halfSizeX, -halfSizeY, halfSizeX, halfSizeY, display.ViewerDistance, pos[0], pos[1] ) );
            raytracer->Render( *raygen, *sampleGen, *sampleAccum );
            sampleAccumCV->SaveToImage( result );
            cv::imwrite( image_filepath, result );
        }
        } break;
    case 2: {
        std::system( "mkdir ProjectorImages_0000" );
        LFRayTracer* raytracer = LFRayTracerPBRTInstance();
        raytracer->LoadScene( argv[1] );
        for ( Int i = 0; i < numProjectors; ++i )
        {
            const Vec3 pos = projectorPositions[i];
            const std::string image_filepath = (ss() << "ProjectorImages_0000" << "/" << std::setfill('0') << std::setw(4) << i << ".exr").str();
            raygen.reset( new DisplayProjectorsCapture( &display, pos ) );
            raytracer->Render( *raygen, *sampleGen, *sampleAccum );
            sampleAccumCV->SaveToImage( result );
            cv::imwrite( image_filepath, result );
        }
        } break;
    case 3: {
        std::system( "mkdir PerceivedImages_0000" );
        DisplayProjectorsShow show( &display );
        if ( !show.LoadScene( "ProjectorImages_0000" ) )
        {
            std::cout << "Could not load projector images! Terminate!" << std::endl;
            return 1;
        }
        for ( Int viewInd = 0; viewInd < numViewerPositions; ++viewInd )
        {
            const Vec3 pos = observerSpace.Position( viewInd );
            const std::string image_filepath = (ss() << "PerceivedImages_0000" << "/" << std::setfill('0') << std::setw(4) << viewInd << ".exr").str();
            raygen.reset( new RayGenPinhole( width, height, -halfSizeX, -halfSizeY, halfSizeX, halfSizeY, display.ViewerDistance, pos[0], pos[1] ) );
            show.Render( *raygen, *sampleGen, *sampleAccum );
            sampleAccumCV->SaveToImage( result );
            cv::imwrite( image_filepath, result );
        }
        } break;
    case 4: {
        std::vector<cv::Mat> zeroIteration( numProjectors );
        std::vector<cv::Mat> groundtrue( numViewerPositions );
        // Load ground-true and initial projector images.
        for ( Int projInd = 0; projInd < numProjectors; ++projInd )
        {
            const std::string image_filepath = (ss() << "ProjectorImages_0000" << "/" << std::setfill('0') << std::setw(4) << projInd << ".exr").str();
            const bool success = LoadImageRGB( image_filepath, zeroIteration[projInd] );
            if ( !success )
            {
                std::cout << "Cannot load projector image: " << image_filepath << std::endl;
                return 1;
            }
        }
        for ( Int viewInd = 0; viewInd < numViewerPositions; ++viewInd )
        {
            const std::string image_filepath = (ss() << "GroundTrueImages" << "/" << std::setfill('0') << std::setw(4) << viewInd << ".exr").str();
            const bool success = LoadImageRGB( image_filepath, groundtrue[viewInd] );
            if ( !success )
            {
                std::cout << "Cannot load ground-true image: " << image_filepath << std::endl;
                return 1;
            }
        }
        // Perform iterations.
        std::vector< std::vector<cv::Mat> > iterations;
        DisplayProjectorsOptimization optimization( &display, &observerSpace );
        const bool success = optimization.Iterate( groundtrue, zeroIteration, iterations, numIterations );
        if ( !success )
        {
            std::cout << "Cannot perform iterations!" << std::endl;
            return 1;
        }
        // Save the result.
        for ( Int iterInd = 0; iterInd < numIterations; ++iterInd )
        {
            const std::string folder_name = (ss() << "ProjectorImages_" << std::setfill('0') << std::setw(4) << (iterInd+1)).str();
            std::system( ("mkdir " + folder_name).c_str() );
            std::vector<cv::Mat>& images = iterations[iterInd];
            for ( Int projInd = 0; projInd < numProjectors; ++projInd )
            {
                const std::string image_filepath = (ss() << folder_name << "/" << std::setfill('0') << std::setw(4) << projInd << ".exr").str();
                cv::imwrite( image_filepath, images[projInd] );
            }
        }
        } break;
    default:
        std::cout << "Your choice is wrong!!! Terminate!" << std::endl;
        break;
    }

    LFRayTRacerPBRTRelease();

    return 0;
}