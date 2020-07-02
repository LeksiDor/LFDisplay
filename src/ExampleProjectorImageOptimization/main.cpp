#include <iostream>

#include <cstdlib>

#include "LFRayTracerPBRT.h"

#include "SampleAccumCV.h"
#include "SampleGenUniform.h"

#include "DisplayProjectorAligned.h"
#include "DisplayProjectorsCapture.h"


DisplayProjectorAligned display;
std::vector<Vec3> projectorPositions;
Int width = 0;
Int height = 0;


using namespace lfrt;
using ss = std::stringstream;


void RenderProjectorImage( const Vec3& position, const std::string& scene_filepath, const std::string& image_filepath )
{
    LFRayTracer* raytracer = LFRayTracerPBRTInstance();

    raytracer->LoadScene( scene_filepath );

    DisplayProjectorsCapture* displayRaygen = new DisplayProjectorsCapture( &display, position );
    std::shared_ptr<const RayGenerator> raygen( displayRaygen );

    std::shared_ptr<SampleGenerator> sampleGen( new SampleGenUniform(3) );

    SampleAccumCV* sampleAccumCV = new SampleAccumCV( width, height );
    std::shared_ptr<SampleAccumulator> sampleAccum( sampleAccumCV );

    raytracer->Render( *raygen, *sampleGen, *sampleAccum );

    cv::Mat result;
    sampleAccumCV->SaveToImage( result );

    cv::imwrite( image_filepath, result );
}



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


    std::cout << std::endl;
    std::cout << "Initializing basic parameters complete." << std::endl;
    std::cout << "What would you like to do?" << std::endl;
    std::cout << "1 - generate projector images" << std::endl << std::endl;

    Int choice = -1;
    std::cin >> choice;

    switch ( choice )
    {
    case 1: {
        std::system( "mkdir ProjectorImages" );
        for ( Int i = 0; i < projectorPositions.size(); ++i )
        {
            const std::string image_filepath = (ss() << "ProjectorImages" << "/" << std::setfill('0') << std::setw(4) << i << ".exr").str();
            RenderProjectorImage( projectorPositions[i], argv[1], image_filepath );
        }
        } break;
    default:
        std::cout << "Your choice is wrong!!! Terminate!" << std::endl;
        break;
    }

    LFRayTRacerPBRTRelease();

    return 0;
}