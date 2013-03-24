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

IF(NOT LLVM_LIT_DIR)
    MESSAGE(FATAL_ERROR "--> Could not locate required LLVM lit Path - Please check ${LLVM_LIT_DIR}")
ELSE()
    file(COPY "${IOC_SOURCE_DIR}/test/lit/astchecker/data" DESTINATION "${IOC_BUILD_DIR}/test/lit/astchecker")
    # Configure a header file to pass some of the CMake settings
    # to the source code

    set(LLVM_LIT_PATH "${LLVM_LIT_DIR}/lit.py")
    set(IOC_ASTCHECKER_TEST_PATH "${IOC_BUILD_DIR}/test/lit/astchecker/data")
IF (WIN32)
    set(IOC_AST_CHECKER_PATH "${IOC_BUILD_DIR}/bin/Debug/astchecker.exe")
    configure_file(
      "${IOC_SOURCE_DIR}/test/lit/test.bat.in"
      "${IOC_BUILD_DIR}/test/lit/test.bat"
      )
ELSE()
    set(IOC_AST_CHECKER_PATH "${IOC_BUILD_DIR}/bin/astchecker")
    configure_file(
      "${IOC_SOURCE_DIR}/test/lit/test.sh.in"
      "${IOC_BUILD_DIR}/test/lit/test.sh"
      )
ENDIF()
    configure_file(
      "${IOC_SOURCE_DIR}/test/lit/astchecker/lit.cfg.in"
      "${IOC_BUILD_DIR}/test/lit/astchecker/lit.cfg"
      )
    execute_process(COMMAND "chmod +x ${IOC_BUILD_DIR}/test/lit/test.sh")
ENDIF()

IF (WIN32)
TARGET_LINK_LIBRARIES(astchecker utils frontend ${ANTLR_LIBRARIES})
ELSE(WIN32)
TARGET_LINK_LIBRARIES(astchecker
  utils
  frontend
  ${ANTLR_LIBRARIES}
  ${Boost_LIBRARIES}
)
ENDIF(WIN32)
