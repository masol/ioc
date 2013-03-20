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
# frontend binary
#
#############################################################################

set(IOC_ANTLR_OUTPUT_DIR "${IOC_BUILD_DIR}/src/frontend")
set(IOC_ANTLR_OUTPUT "${IOC_ANTLR_OUTPUT_DIR}/javascriptParser.c")

ADD_CUSTOM_COMMAND(
	OUTPUT ${IOC_ANTLR_OUTPUT}
	# Creating custom doxygen-ioc.conf
	COMMAND if exist "${IOC_ANTLR_OUTPUT_DIR}" echo "dir exist" else mkdir -p "${IOC_ANTLR_OUTPUT_DIR}"
	
	COMMAND "${Java_JAVA_EXECUTABLE}" -cp "${ANTLR_JAR_PATH}" org.antlr.Tool "${IOC_SOURCE_DIR}/src/frontend/javascript.g"
	WORKING_DIRECTORY "${IOC_ANTLR_OUTPUT_DIR}"
    DEPENDS "${IOC_SOURCE_DIR}/src/frontend/javascript.g"
)

ADD_CUSTOM_COMMAND(
   OUTPUT ${IOC_ANTLR_OUTPUT_DIR}/javascriptLexer.c
   COMMAND ${CMAKE_COMMAND} -E echo "javascriptLexer.c is up to date"
   DEPENDS ${IOC_ANTLR_OUTPUT}
   )

set(FRONTEND_SRCS
	antlrhelper.cpp
	astfactory.cpp
#	${IOC_ANTLR_OUTPUT_DIR}/javascriptParser.c
#	${IOC_ANTLR_OUTPUT_DIR}/javascriptLexer.c
	)

include_directories("${ANTLR_INCLUDE_DIR}")

add_library(frontend STATIC ${FRONTEND_SRCS})