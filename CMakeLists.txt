cmake_minimum_required (VERSION 2.8)

project ( LFDisplay )

find_package ( OpenCV REQUIRED )

set ( CMAKE_CXX_STANDARD 17 )

add_definitions ( -D _USE_MATH_DEFINES )

add_subdirectory( src/ExampleNearEyeInImRT )
add_subdirectory( src/Utilities )