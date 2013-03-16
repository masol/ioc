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

MESSAGE(STATUS "CMAKE VERSION DETECTED " ${CMAKE_VERSION})

###########################################################################
#
# Check and configure cmake
#
###########################################################################
# Fresh start
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
#Remove the following when the version check is at least 2.8.4
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(ioc)

# Dont overconfigure
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limit configs" FORCE)

###########################################################################
#
# Include necessary submodules
#
###########################################################################

set(CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/cmake"
	"${PROJECT_SOURCE_DIR}/cmake/Utils"
	"${PROJECT_SOURCE_DIR}/cmake/Packages"
	"${PROJECT_SOURCE_DIR}/cmake/SpecializedConfig"
)

INCLUDE(PlatformSpecific)
INCLUDE(Configuration)

# Install CMake modules
#add_subdirectory(CMake)

SET(LuxRays_INCLUDE_DIR "${LuxRays_SOURCE_DIR}/include")
include_directories("${LuxRays_INCLUDE_DIR}")

# When using single configuration generators like make
# cmake does need to know which of the possible configurations
# to generate
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "assure config" FORCE) # makes sure type is shown in cmake gui

message(STATUS "Building mode: " ${CMAKE_BUILD_TYPE})

# Find dependencies
include(Dependencies)

if (NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required Boost files - Please check ${BOOST_SEARCH_PATH}")
endif()

#if (NOT V8_FOUND)
#	MESSAGE(FATAL_ERROR "--> Could not locate required v8 files - Please check ${V8_SEARCH_PATH}")
#endif()

if (NOT Java_Runtime_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required Java Environment - Please check ${Java_JAVA_EXECUTABLE}")
endif()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src/ioc)

###########################################################################
#
# For non win32 we'll have to copy everything to a single dir
#
###########################################################################

INCLUDE(AssembleBinDirs)
