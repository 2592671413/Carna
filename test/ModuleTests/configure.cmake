include_directories( ${CMAKE_PROJECT_DIR}ModuleTests )

list( APPEND TESTS
		DRRStageTest
	)

list( APPEND TESTS_QOBJECT_HEADERS
		ModuleTests/DRRStageTest.h
	)

list( APPEND TESTS_HEADERS
		""
	)

list( APPEND TESTS_SOURCES
		ModuleTests/DRRStageTest.cpp
	)
