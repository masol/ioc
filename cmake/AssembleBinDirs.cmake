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
# Binary samples directory
#
###########################################################################

IF (WIN32)
	
  # For MSVC moving exe files gets done automatically
  # If there is someone compiling on windows and
  # not using msvc (express is free) - feel free to implement

ELSE (WIN32)
	set(CMAKE_INSTALL_PREFIX .)

ENDIF(WIN32)