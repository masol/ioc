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

#############################################################################
#
# IOC binary
#
#############################################################################

find_package(Doxygen)
if(DOXYGEN_FOUND)
	message(STATUS "Found Doxygen and generating ioc documentation")
	
	# Generate doxygen.template
	set(DOXYGEN_IOC_TEMPLATE ${IOC_BUILD_DIR}/doxygen/ioc.template)
	configure_file(
	  "${IOC_SOURCE_DIR}/doxygen/ioc.template.in"
	  "${DOXYGEN_IOC_TEMPLATE}"
	  )

	#message(STATUS "IOC_BUILD_DIR: " ${IOC_BUILD_DIR})

	set(DOXYGEN_IOC_INPUT ${IOC_BUILD_DIR}/doxygen-ioc.conf)
	set(DOXYGEN_IOC_OUTPUT_DIR ${IOC_BUILD_DIR}/doc/ioc)
	set(DOXYGEN_IOC_OUTPUT ${DOXYGEN_IOC_OUTPUT_DIR}/html/index.html)

	message(STATUS "Doxygen ioc output: " ${DOXYGEN_IOC_OUTPUT})

	if(DOXYGEN_DOT_FOUND)
			message(STATUS "Found dot")
			set(DOXYGEN_DOT_CONF "HAVE_DOT = YES")
	endif(DOXYGEN_DOT_FOUND)
	
	IF (WIN32)
		STRING(REGEX REPLACE "/" "\\\\" DOXYGEN_IOC_OUTPUT_DIR ${DOXYGEN_IOC_OUTPUT_DIR})
		STRING(REGEX REPLACE "/" "\\\\" DOXYGEN_IOC_TEMPLATE ${DOXYGEN_IOC_TEMPLATE})
		STRING(REGEX REPLACE "/" "\\\\" DOXYGEN_IOC_INPUT ${DOXYGEN_IOC_INPUT})
		
		SET(DOXYGEN_CREATE_OUTPUT_DIR if exist "${DOXYGEN_IOC_OUTPUT_DIR}" echo "dir exist" else mkdir -p "${DOXYGEN_IOC_OUTPUT_DIR}")
		SET(DOXYGEN_COPY_TEMPLATE copy /Y "${DOXYGEN_IOC_TEMPLATE}" "${DOXYGEN_IOC_INPUT}")
	ELSE()
	ENDIF(WIN32)

	add_custom_command(
		OUTPUT ${DOXYGEN_IOC_OUTPUT}
		# Creating custom doxygen-ioc.conf
		COMMAND ${DOXYGEN_CREATE_OUTPUT_DIR}
		COMMAND ${DOXYGEN_COPY_TEMPLATE}
		COMMAND echo INPUT = "${IOC_SOURCE_DIR}/include"  "${IOC_SOURCE_DIR}/src" >> ${DOXYGEN_IOC_INPUT}
		COMMAND echo OUTPUT_DIRECTORY = "${DOXYGEN_IOC_OUTPUT_DIR}" >> ${DOXYGEN_IOC_INPUT}
		IF (DOXYGEN_DOT_CONF)
			COMMAND echo ${DOXYGEN_DOT_CONF} >> ${DOXYGEN_IOC_INPUT}
		ENDIF(DOXYGEN_DOT_CONF)
		# Launch doxygen
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IOC_INPUT}
		DEPENDS ${DOXYGEN_IOC_TEMPLATE}
		WORKING_DIRECTORY ${IOC_SOURCE_DIR}
	)

	add_custom_target(apidoc_ioc DEPENDS ${DOXYGEN_IOC_OUTPUT})
endif(DOXYGEN_FOUND)

set(IOC_SRCS
	main.cpp
	)

add_executable(ioc ${IOC_SRCS})

IF (WIN32)
TARGET_LINK_LIBRARIES(ioc frontend utils ${ANTLR_LIBRARIES})
ELSE(WIN32)
TARGET_LINK_LIBRARIES(ioc
  frontend
  utils
  ${ANTLR_LIBRARIES} 
  ${Boost_LIBRARIES}
)
ENDIF(WIN32)
