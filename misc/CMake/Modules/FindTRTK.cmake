############################################
# Locates TRTK library.
# ------------------------------------------
# If TRTK headers and library files are not
# found in the standard CMake locations,
# then %MEDITEC_LIBS%/TRTK/<version> is
# checked. In this, letter case it is
# necessary that the version is matched
# exactly. Consider using symbolic links
# when you have a compatible version.
#-------------------------------------------
# The following variables are set:
#	TRTK_INCLUDE_DIR	points to headers
#	TRTK_LIBRARY		points to lib file
#	TRTK_FOUND			indicates success
#-------------------------------------------
# Leonid Kostrykin, 12.12.2014
############################################

find_path(	TRTK_INCLUDE_DIR
			TRTK/Version.hpp
			PATHS ENV MEDITEC_LIBS
			PATH_SUFFIXES "TRTK/${TRTK_FIND_VERSION}/include"
			DOC "TRTK headers")

find_library(	TRTK_LIBRARY
				NAMES "TRTK-${TRTK_FIND_VERSION}" TRTK
				PATHS ENV MEDITEC_LIBS
				PATH_SUFFIXES "TRTK/${TRTK_FIND_VERSION}/bin"
				DOC "TRTK library")

# handles REQUIRED, QUIET and version-related
# arguments and also sets the _FOUND variable
find_package_handle_standard_args(TRTK
	REQUIRED_VARS TRTK_INCLUDE_DIR TRTK_LIBRARY)