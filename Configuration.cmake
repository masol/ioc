###########################################################################
#   Copyright (C) 2012-2013 by authors (see AUTHORS.txt)                  #
#                                                                         #
#   This file is part of IOC.                                             #
#                                                                         #
#   IOC is free software; you can redistribute it and/or modify it under  #
#   the terms of the IOC Public License(LGPL License) as published by     #
#   masol.lee.                                                            #
#                                                                         #
#   IOC is distributed in the hope that it will be useful,but WITHOUT     #
#   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY    #
#   or FITNESS FOR A PARTICULAR PURPOSE.                                  #
#                                                                         #
#   You should have received a copy of the IOC License along with this    #
#   program.  If not, see <http://www.masols.com/licenses/>.              #
#                                                                         #
#   IOC website: http://www.masols.com                                    #
###########################################################################

###########################################################################
#
# Configuration
#
# Use cmake "-DIOC_CUSTOM_CONFIG=YouFileCName" To define your personal settings
# where YouFileCName.cname must exist in one of the cmake include directories
# best to use cmake/SpecializedConfig/
# 
# To not load defaults before loading custom values define
# -DIOC_NO_DEFAULT_CONFIG=true
#
# WARNING: These variables will be cached like any other
#
###########################################################################

IF (NOT IOC_NO_DEFAULT_CONFIG)

  # Disable Boost automatic linking
  # ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

  IF (WIN32)

    MESSAGE(STATUS "Using default WIN32 Configuration settings")

    IF(MSVC)

      STRING(REGEX MATCH "(Win64)" _carch_x64 ${CMAKE_GENERATOR})
      IF(_carch_x64)
        SET(WINDOWS_ARCH "x64")
      ELSE(_carch_x64)
        SET(WINDOWS_ARCH "x86")
      ENDIF(_carch_x64)
      MESSAGE(STATUS "Building for target ${WINDOWS_ARCH}")
	  
      SET(MSVC_RT_DEBUG_FIX 1)

    ELSE(MSVC)

    ENDIF(MSVC)

    IF(DEFINED ENV{IOC_DEP_DIRS})
	SET(IOC_DEP_DIRS $ENV{IOC_DEP_DIRS})
	MESSAGE(STATUS "ioc build environment variables found")
	SET(BOOST_SEARCH_PATH         "${IOC_DEP_DIRS}/boost_1_53_0")
	SET(Java_JAVA_EXECUTABLE    	"C:\\Program Files\\Java\\jre7\\bin\\java.exe")
#	SET(ANTLR_JAR_PATH    	"${IOC_DEP_DIRS}/javalib/antlr-3.4-complete.jar")
	SET(ANTLR_JAR_PATH    	"${IOC_DEP_DIRS}/javalib/antlr-3.2.jar")
    ENDIF(DEFINED ENV{IOC_DEP_DIRS})

  ELSE(WIN32)
    IF(DEFINED ENV{IOC_DEP_DIRS})
	SET(IOC_DEP_DIRS $ENV{IOC_DEP_DIRS})
	MESSAGE(STATUS "ioc build environment variables found")
	SET(Java_JAVA_EXECUTABLE    	"/usr/bin/java")
	SET(ANTLR_JAR_PATH    	"${IOC_DEP_DIRS}/javalib/antlr-3.2.jar")
    ENDIF(DEFINED ENV{IOC_DEP_DIRS})
  ENDIF(WIN32)

ELSE(NOT IOC_NO_DEFAULT_CONFIG)
	
  MESSAGE(STATUS "IOC_NO_DEFAULT_CONFIG defined - not using default configuration values.")

ENDIF(NOT IOC_NO_DEFAULT_CONFIG)

# Setup libraries output directory
SET (LIBRARY_OUTPUT_PATH
   ${PROJECT_BINARY_DIR}/lib
   CACHE PATH
   "Single Directory for all Libraries"
   )

# Setup binaries output directory
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single Directory for all binaries"
	)

#
# Overwrite defaults with Custom Settings
#

IF (IOC_CUSTOM_CONFIG)
	MESSAGE(STATUS "Using custom build config: ${IOC_CUSTOM_CONFIG}")
	INCLUDE(${IOC_CUSTOM_CONFIG})
ENDIF (IOC_CUSTOM_CONFIG)

