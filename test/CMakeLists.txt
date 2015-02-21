cmake_minimum_required(VERSION 2.8.7)
	
set( TESTS_QOBJECT_HEADERS
		""
	)

set( TESTS_HEADERS
		Tools/FramebufferTester.h
		Tools/HUGZSceneFactory.h
		Tools/HUIO.h
		Tools/MockSceneFactory.h
		Tools/MyVisualization.h
	)
	
set( TESTS_SOURCES
		Tools/FramebufferTester.cpp
		Tools/MockSceneFactory.cpp
		Tools/MyVisualization.cpp
	)
	
set( TESTS
		""
	)
	
set( TESTS_MISC_FILES
		UnitTests/configure.cmake
		ModuleTests/configure.cmake
	)
	
set( TESTS_RESOURCES
		res/histograms.qrc
	)

include( UnitTests/configure.cmake )
include( ModuleTests/configure.cmake )

############################################
# Compose test suite
############################################

QT4_WRAP_CPP( TESTS_QOBJECT_HEADERS_MOC ${TESTS_QOBJECT_HEADERS} )
QT4_ADD_RESOURCES( TESTS_RESOURCES_RCC ${TESTS_RESOURCES} )

set( TEST_SUITE_SRC_FILE	${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-testsuite.cpp )

file( REMOVE	${TEST_SUITE_SRC_FILE} )
file( WRITE		${TEST_SUITE_SRC_FILE}	"// This file is automatically generated by CMake.\n\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"#include <QtTest/QtTest>\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"#include \"TestSuite.h\"\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"const std::string SOURCE_PATH = \"${CMAKE_CURRENT_SOURCE_DIR}\";\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"const std::string BINARY_PATH = \"${CMAKE_CURRENT_BINARY_DIR}\";\n" )

foreach( SOURCE_FILE ${TESTS_SOURCES} )
	file( APPEND	${TEST_SUITE_SRC_FILE}	"#include \"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}\"\n" )
endforeach( SOURCE_FILE )

foreach( QT_GENERATED_SOURCE_FILE ${TESTS_QOBJECT_HEADERS_MOC} ${TESTS_RESOURCES_RCC} )
	file( APPEND	${TEST_SUITE_SRC_FILE}	"#include \"${QT_GENERATED_SOURCE_FILE}\"\n" )
endforeach( QT_GENERATED_SOURCE_FILE )

file( APPEND	${TEST_SUITE_SRC_FILE}	"\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"#include <QApplication>\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"#include <Carna/base/CarnaException.h>\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"int main( int argc, char** argv )\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"{\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"    QApplication app( argc, argv );\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"    Carna::base::BreakForDebug::disable();\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"    bool success = true;\n" )
foreach( TEST_CASE ${TESTS} )
	file( APPEND	${TEST_SUITE_SRC_FILE}	"\n" )
	file( APPEND	${TEST_SUITE_SRC_FILE}	"    ${TEST_CASE} instance_of_${TEST_CASE};\n" )
	file( APPEND	${TEST_SUITE_SRC_FILE}	"    success &= ( QTest::qExec( &instance_of_${TEST_CASE}, argc, argv ) == 0 );\n" )
	file( APPEND	${TEST_SUITE_SRC_FILE}	"\n" )
endforeach( TEST_CASE )
file( APPEND	${TEST_SUITE_SRC_FILE}	"    return success ? 0 : -1;\n" )
file( APPEND	${TEST_SUITE_SRC_FILE}	"}\n" )
	
############################################
# Setup Visual Studio filters
############################################

SOURCE_GROUP( "Unit Tests"
	REGULAR_EXPRESSION \(.*test/UnitTests/.*\\.\)\(\(h\)|\(cpp\)|\(cmake\)\) )

SOURCE_GROUP( "Module Tests"
	REGULAR_EXPRESSION \(.*test/ModuleTests/.*\\.\)\(\(h\)|\(cpp\)|\(cmake\)\) )

SOURCE_GROUP( "Miscellaneous"
	REGULAR_EXPRESSION .*\\.\(\(in\)|\(txt\)|\(qrc\)\) )

SOURCE_GROUP( "Miscellaneous\\Generated Files"
	FILES "${TEST_SUITE_SRC_FILE}" ${TESTS_QOBJECT_HEADERS_MOC} ${TESTS_RESOURCES_RCC} )

SOURCE_GROUP( "Miscellaneous\\Tools"
	REGULAR_EXPRESSION \(.*test/Tools/.*\\.\)\(\(h\)|\(cpp\)\) )

############################################
# Find dependencies and add them to the linker
############################################

# Qt
find_package( Qt4 4.8.0 COMPONENTS QtTest QtOpenGL REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

# Boost
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.55.0 COMPONENTS iostreams)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# GLEW
find_package( GLEW 1.7.0 REQUIRED )
include_directories( ${GLEW_INCLUDE_DIRS} )

# Carna
include_directories( ${CMAKE_SOURCE_DIR}/include )
link_directories( ${CMAKE_BINARY_DIR}/bin )

# Eigen
find_package( Eigen3 REQUIRED )
include_directories( ${EIGEN3_INCLUDE_DIR} )

# TRTK
find_package( TRTK 0.13.1 REQUIRED )
include_directories( ${TRTK_INCLUDE_DIR} )

############################################
# Project
############################################

if(Boost_FOUND)

	remove_definitions( -DCARNA_LIB )
	add_definitions( -DBOOST_IOSTREAMS_NO_LIB )

	include_directories( ${CMAKE_PROJECT_DIR}Tools )

	set_source_files_properties( ${TESTS_QOBJECT_HEADERS_MOC} ${TESTS_SOURCES} ${TESTS_MISC_FILES} ${TESTS_RESOURCES_RCC}
		PROPERTIES
		HEADER_FILE_ONLY TRUE )

	add_executable( ${TARGET_NAME}-testsuite
			Tools/TestSuite.h
			${TEST_SUITE_SRC_FILE}
			${TESTS_HEADERS}
			${TESTS_QOBJECT_HEADERS}
			${TESTS_QOBJECT_HEADERS_MOC}
			${TESTS_SOURCES}
			${TESTS_MISC_FILES}
			${TESTS_RESOURCES_RCC}
		)
			
	target_link_libraries( ${TARGET_NAME}-testsuite
			opengl32
			glu32
			${GLEW_LIBRARIES}
			${QT_LIBRARIES}
			${TRTK_LIBRARIES}
			${Boost_LIBRARIES}
			optimized	${TARGET_NAME}
			debug		${TARGET_NAME}${CMAKE_DEBUG_POSTFIX}
		)

	############################################
	# Run the tests
	############################################

	# CMake 2.8.12 and lower allowed reading the LOCATION target property (and 
	# configuration-specific variants) to determine the eventual location of build 
	# targets. This relies on the assumption that all necessary information is 
	# available at configure-time to determine the final location and filename of the 
	# target. However, this property is not fully determined until later at 
	# generate-time. [...] The OLD behavior for this policy is to allow reading the 
	# LOCATION properties from build-targets. The NEW behavior for this policy is to 
	# not to allow reading the LOCATION properties from build-targets.
	#
	# --> http://www.cmake.org/cmake/help/v3.0/policy/CMP0026.html
	if( POLICY CMP0026 )
		cmake_policy(SET CMP0026 OLD)
	endif()

	get_property( TEST_SUITE_EXECUTABLE TARGET ${TARGET_NAME}-testsuite PROPERTY LOCATION )
	add_custom_target( RUN_TESTSUITE
			ALL
			${TEST_SUITE_EXECUTABLE}
			DEPENDS ${TARGET_NAME} ${TARGET_NAME}-testsuite
			COMMENT "Running test suite..."
		)
	
else(Boost_FOUND)

	message(WARNING "\nBoost not found, skipping testsuite.\n")

endif(Boost_FOUND)