#include <iostream>

#include "DisplayProjectorAligned.h"


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


    DisplayProjectorAligned display;
    if ( !display.Load( "../../data/display-projectors-aligned.yaml" ) )
    {
        std::cout << "Cannot load display model." << std::endl;
        return 1;
    }

    return 0;
}