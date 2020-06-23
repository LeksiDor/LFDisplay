set ( TARGET_NAME Utilities )

file ( GLOB SOURCE_FILES "*.cpp" )
file ( GLOB HEADER_FILES "*.h" )
file ( GLOB COMMON_FILES "../*.h" "../*.cpp" )

add_library ( ${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${COMMON_FILES} )

source_group ( "Sources" FILES ${HEADER_FILES} ${SOURCE_FILES} )
source_group ( "Common" FILES ${COMMON_FILES} )

set_target_properties(${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

target_include_directories ( ${TARGET_NAME}
	PUBLIC ${OpenCV_INCLUDE_DIRS}
	PUBLIC ${PROJECT_SOURCE_DIR}/src/
	)

target_link_libraries( ${TARGET_NAME}
	${OpenCV_LIBS}
	)