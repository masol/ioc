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
# astchecker binary.
# AstChecker is a small tool used to check the correctness of parsing.
#
#############################################################################

set(ASTCHECKER_SRCS
	main.cpp
	)

add_executable(astchecker ${ASTCHECKER_SRCS})

IF (WIN32)
TARGET_LINK_LIBRARIES(astchecker frontend utils ${ANTLR_LIBRARIES})
ELSE(WIN32)
TARGET_LINK_LIBRARIES(astchecker
  frontend
  utils
  ${ANTLR_LIBRARIES}
  ${Boost_LIBRARIES}
)
ENDIF(WIN32)
