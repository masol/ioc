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
# CppUnit Test binary
#
#############################################################################

set(TEST_BASE_SRCS
	main.cpp
	JUnitXmlOutputter.cpp
	JUnitTestResultCollector.cpp
	)
	
set(TEST_CASE_SRCS
	utils/zone_test.cpp
	utils/option_test.cpp
	utils/log_test.cpp
	)


include_directories(${CPPUNIT_INCLUDE_DIR})
add_executable(ioc_test ${TEST_BASE_SRCS} ${TEST_CASE_SRCS})

if(WIN32)
TARGET_LINK_LIBRARIES(ioc_test utils ${CPPUNIT_LIBRARIES})
else(WIN32)
message(STATUS "${Boost_LIBRARIES}")
TARGET_LINK_LIBRARIES(ioc_test utils ${CPPUNIT_LIBRARIES} ${Boost_LIBRARIES})
endif(WIN32)