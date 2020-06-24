set ( TARGET_NAME ExampleLenslet )

file ( GLOB SOURCE_FILES "*.cpp" )
file ( GLOB HEADER_FILES "*.h" )
file ( GLOB COMMON_FILES "../*.h" "../*.cpp" )

add_executable ( ${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${COMMON_FILES} )

source_group ( "Sources" FILES ${HEADER_FILES} ${SOURCE_FILES} )
source_group ( "Common" FILES ${COMMON_FILES} )

set_target_properties ( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )

add_dependencies( ${TARGET_NAME} Utilities )

target_include_directories ( ${TARGET_NAME}
	PUBLIC ${OpenCV_INCLUDE_DIRS}
	PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/lfraytracer/include
	PUBLIC ${PROJECT_SOURCE_DIR}/src
	PUBLIC ${PROJECT_SOURCE_DIR}/src/Utilities
	)

target_link_libraries( ${TARGET_NAME}
	${OpenCV_LIBS}
	debug ${PROJECT_SOURCE_DIR}/bin/Debug/Utilities.lib                      optimized ${PROJECT_SOURCE_DIR}/bin/Release/Utilities.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/lfraytracer/lib/Debug/libpbrt.lib       optimized ${PROJECT_SOURCE_DIR}/3rdparty/lfraytracer/lib/Release/libpbrt.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/glog/lib/Debug/glog.lib             optimized ${PROJECT_SOURCE_DIR}/3rdparty/glog/lib/Release/glog.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/ptex/lib/Debug/Ptex.lib             optimized ${PROJECT_SOURCE_DIR}/3rdparty/ptex/lib/Release/Ptex.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/zlib/lib/Debug/zlibstatic.lib       optimized ${PROJECT_SOURCE_DIR}/3rdparty/zlib/lib/Release/zlibstatic.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Debug/Half.lib          optimized ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Release/Half.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Debug/Iex.lib           optimized ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Release/Iex.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Debug/IlmImf.lib        optimized ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Release/IlmImf.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Debug/IlmThread.lib     optimized ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Release/IlmThread.lib
	debug ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Debug/Imath.lib         optimized ${PROJECT_SOURCE_DIR}/3rdparty/openexr/lib/Release/Imath.lib
	)