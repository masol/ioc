# from https://github.com/nu7hatch/mike/blob/master/cmake/FindV8.cmake
#
# FindV8.cmake - This module finds V8 library.
#
# It sets the following variables:
#  V8_FOUND       - Set to false, or undefined, if V8 isn't found.
#  V8_INCLUDE_DIR - The V8 include directory.
#  V8_LIBRARY     - The V8 library to link against.

IF (WIN32)
	FIND_PATH( V8_INCLUDE_DIR v8.h
		${WITH_V8}/include
		${V8_ROOT}/include
		$ENV{UGP_DEP_DIRS}/v8/include
		${PROJECT_SOURCE_DIR}/src/v8/include
		DOC "The directory where v8.h resides")
	FIND_LIBRARY( V8_LIBRARY
		NAMES v8_base v8_nosnapshot v8 v8_snapshot mksnapshot
		PATHS
		${V8_LIBRARYDIR}
		${WITH_V8}/build/Release/lib
		${V8_ROOT}/build/Release/lib
		$ENV{UGP_DEP_DIRS}/v8/build/Release/lib
		${PROJECT_SOURCE_DIR}/src/v8/build/Release/lib
		DOC "The Lib library")
ELSE (WIN32)
ENDIF (WIN32)

IF (V8_INCLUDE_DIR)
	SET( V8_FOUND 1 CACHE STRING "Set to 1 if V8 is found, 0 otherwise")
ELSE (V8_INCLUDE_DIR)
	SET( V8_FOUND 0 CACHE STRING "Set to 1 if V8 is found, 0 otherwise")
ENDIF (V8_INCLUDE_DIR)

MARK_AS_ADVANCED( V8_FOUND )
