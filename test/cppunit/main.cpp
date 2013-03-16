////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2012-2013 by authors (see AUTHORS.txt)                  //
//                                                                        //
//  This file is part of IOC.                                             //
//                                                                        //
//  IOC is free software; you can redistribute it and//or modify it under //
//  the terms of the IOC Public License(LGPL License) as published by     //
//  masol.lee.                                                            //
//                                                                        //
//  IOC is distributed in the hope that it will be useful,but WITHOUT     //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY    //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                  //
//                                                                        //
//  You should have received a copy of the IOC License along with this    //
//  program.  If not, see <http://www.masols.com/licenses>.               //
//                                                                        //
//  IOC website: http://www.masols.com                                    //
////////////////////////////////////////////////////////////////////////////

#include    <cppunit/extensions/TestFactoryRegistry.h>
#include    <cppunit/ui/text/TestRunner.h>
#include 	<cppunit/CompilerOutputter.h>
#include 	<cppunit/extensions/TestFactoryRegistry.h>
#include 	<cppunit/TestResult.h>
#include 	<cppunit/TestResultCollector.h>
#include 	<cppunit/TestRunner.h>
#include 	<cppunit/BriefTestProgressListener.h>
#include 	<boost/filesystem/fstream.hpp>
#include    <stdlib.h>
#include    "JUnitTestResultCollector.h"
#include    "JUnitXmlOutputter.h"

/** 
 * @brief           main function
 **/
int main ( int argc, char *argv[] )
{
	CPPUNIT_NS :: TestResult testresult;
	CPPUNIT_NS :: TestRunner runner;
	//CppUnit::TextUi::TestRunner runner;
	CPPUNIT_NS :: JUnitTestResultCollector collectedresults;

	CppUnit::TestFactoryRegistry & registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest( registry.makeTest() );
		
	testresult.addListener (&collectedresults);
	runner.run(testresult);
	 // output results in compiler-format
	if (argc>1)
	{//if give output file name 如果命令行带参数,就把这个参数当作是要输出的文件名
			boost::filesystem::ofstream ofs(argv[1]);
			if(ofs.is_open())
			{// output to file
		    CPPUNIT_NS :: JUnitXmlOutputter JUnitoutputter (&collectedresults, ofs);
		    JUnitoutputter.write ();
		    
		    return collectedresults.wasSuccessful () ? 0 : 1;
			}	
				
	}
	//output to stderr
    CPPUNIT_NS :: JUnitXmlOutputter JUnitoutputter (&collectedresults, std::cerr);
    JUnitoutputter.write ();
	
	return collectedresults.wasSuccessful () ? 0 : 1;
}               /* ----------  end of function main  ---------- */


