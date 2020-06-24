cmake_minimum_required (VERSION 2.8)

project ( LFDisplay )


OPTION(PBRT_FLOAT_AS_DOUBLE "PBRT uses 64-bit floats" OFF)


IF (PBRT_FLOAT_AS_DOUBLE)
  ADD_DEFINITIONS ( -D PBRT_FLOAT_AS_DOUBLE )
  ADD_DEFINITIONS ( -D LFRT_REAL_AS_DOUBLE )
ENDIF()


find_package ( OpenCV REQUIRED )

set ( CMAKE_CXX_STANDARD 17 )

add_definitions ( -D _USE_MATH_DEFINES )

add_subdirectory( src/ExampleLenslet )
add_subdirectory( src/ExampleNearEyeInImRT )
add_subdirectory( src/Utilities )