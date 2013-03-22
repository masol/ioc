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

# - Try to find the antlr library (http://www.antlr.org/)
#
# Once done this will define
#
# ANTLR_FOUND - system has libantlr
# ANTLR_INCLUDE_DIR - the libantlr include directory
# ANTLR_LIBRARIES - Link these to use libantlr
#

if (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARIES AND ANTLR_LIBRARIES_DIR)

    # in cache already
    set(ANTLR_FOUND TRUE)

else (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARIES AND ANTLR_LIBRARIES_DIR)
	
	if(NOT ANTLR_INCLUDE_DIR)
		set(ANTLR_INCLUDE_DIR ${IOC_DEP_DIRS}/javalib/antlr3-antlr-3.5/runtime/C/include)
	endif(NOT ANTLR_INCLUDE_DIR)

	if(NOT ANTLR_LIBRARIES_DIR)
		set(ANTLR_LIBRARIES_DIR ${IOC_DEP_DIRS}/javalib/antlr3-antlr-3.5/runtime/C)
		if(_carch_x64)
			set(ANTLR_LIBRARIES_DIR ${ANTLR_LIBRARIES_DIR}/x64/Release)
		else(_carch_x64)
			set(ANTLR_LIBRARIES_DIR ${ANTLR_LIBRARIES_DIR}/Release)
		endif(_carch_x64)
	endif(NOT ANTLR_LIBRARIES_DIR)
	
    find_path(ANTLR_INCLUDE_DIR antlr3.h)

    find_library(ANTLR_LIBRARIES NAMES antlr3c antlr3 libantlr3 libantlr3c PATHS ${ANTLR_LIBRARIES_DIR} EVN path)

    if (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARIES)
        set(ANTLR_FOUND TRUE)
        # TODO version check is missing
    endif (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARIES)

    if (ANTLR_FOUND)
        if (NOT ANTLR_FIND_QUIETLY)
            message(STATUS "Found Antlr3 C runtime: ${ANTLR_LIBRARIES}")
        endif (NOT ANTLR_FIND_QUIETLY)
    else (ANTLR_FOUND)
        if (ANTLR_FIND_REQUIRED)
            if (NOT ANTLR_INCLUDE_DIR)
                message(FATAL_ERROR "Could NOT find Antlr3 header files")
            endif (NOT ANTLR_INCLUDE_DIR)
            if (NOT ANTLR_LIBRARIES)
                message(FATAL_ERROR "Could NOT find Antlr3 C runtime library")
            endif (NOT ANTLR_LIBRARIES)
        endif (ANTLR_FIND_REQUIRED)
    endif (ANTLR_FOUND)

    mark_as_advanced(ANTLR_INCLUDE_DIR ANTLR_LIBRARIES)
  
endif (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARIES AND ANTLR_LIBRARIES_DIR)