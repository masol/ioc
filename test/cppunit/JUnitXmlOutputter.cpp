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


#include <cppunit/Exception.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include "JUnitXmlOutputter.h"
#include <cppunit/XmlOutputterHook.h>
#include <cppunit/tools/XmlDocument.h>
#include <cppunit/tools/XmlElement.h>
#include <stdlib.h>
#include <algorithm>
#include <list>
#include <boost/unordered_map.hpp>

CPPUNIT_NS_BEGIN


JUnitXmlOutputter::JUnitXmlOutputter( JUnitTestResultCollector *result,
                            OStream &stream,
                            std::string encoding )
  : m_result( result )
  , m_stream( stream )
  , m_xml( new XmlDocument( encoding ) )
  , m_hostname("localhost")
  , m_name("unit_test")
{
}


JUnitXmlOutputter::~JUnitXmlOutputter()
{
  delete m_xml;
}


void 
JUnitXmlOutputter::addHook( XmlOutputterHook *hook )
{
  m_hooks.push_back( hook );
}


void 
JUnitXmlOutputter::removeHook( XmlOutputterHook *hook )
{
  m_hooks.erase( std::find( m_hooks.begin(), m_hooks.end(), hook ) );
}


void 
JUnitXmlOutputter::write()
{
  setRootNode();
  m_stream  <<  m_xml->toString();
}


void 
JUnitXmlOutputter::setStyleSheet( const std::string &styleSheet )
{
  m_xml->setStyleSheet( styleSheet );
}


void
JUnitXmlOutputter::setStandalone( bool standalone )
{
  m_xml->setStandalone( standalone );
}
 

XmlElement*
JUnitXmlOutputter::getTestSuiteNode(XmlElement *pRootNode,const std::string& name,size_t testCounts)
{
	XmlElement *pNode = NULL;//pRootNode->elementFor(name);
	if(!pNode)
	{
		pNode = new XmlElement( "testsuite" );
		pRootNode->addElement(pNode);
		
		char buf[100];

		double sec = (double) m_result->getAllTestsDuration() / CLOCKS_PER_SEC;
		sprintf(buf, "%0.04f", sec);
		pNode->addAttribute("time", buf);

		time_t mytimet;
		time(&mytimet);
		struct tm * mytime = localtime(&mytimet);
		strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", mytime);
		pNode->addAttribute( "timestamp", buf );

		sprintf(buf, "%d", testCounts);
		pNode->addAttribute("tests", buf);

		pNode->addAttribute("hostname", m_hostname);
		pNode->addAttribute("name", name);
		
	}
	return pNode;
}

void
JUnitXmlOutputter::setRootNode()
{
    XmlElement *rootNode = new XmlElement( "testsuites" );
    m_xml->setRootElement( rootNode );

    for ( Hooks::iterator it = m_hooks.begin(); it != m_hooks.end(); ++it ) {
        (*it)->beginDocument( m_xml );
    }

    FailedTests failedTests;
    fillFailedTestsMap( failedTests );

    char buf[100];
 
    double sec = (double) m_result->getAllTestsDuration() / CLOCKS_PER_SEC;
    sprintf(buf, "%0.04f", sec);
    rootNode->addAttribute("time", buf);

    time_t mytimet;
    time(&mytimet);
    struct tm * mytime = localtime(&mytimet);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", mytime);
    rootNode->addAttribute( "timestamp", buf );

    int countErrors = 0, countFailures = 0;
    for (FailedTests::iterator i = failedTests.begin(); i != failedTests.end(); ++i) {
        if (i->second->isError()) 
            ++countErrors;
        else
            ++countFailures;
    }

    sprintf(buf, "%d", countErrors);
    rootNode->addAttribute("errors", buf);

    sprintf(buf, "%d", countErrors);
    rootNode->addAttribute("failures", buf);

    sprintf(buf, "%d", m_result->tests().size());
    rootNode->addAttribute("tests", buf);

    rootNode->addAttribute("hostname", m_hostname);
    rootNode->addAttribute("name", m_name);

    addAllTests( failedTests, rootNode );

    for ( Hooks::iterator itEnd = m_hooks.begin(); itEnd != m_hooks.end(); ++itEnd ) {
        (*itEnd)->endDocument( m_xml );
    }
}


void 
JUnitXmlOutputter::fillFailedTestsMap( FailedTests &failedTests )
{
  const JUnitTestResultCollector::TestFailures &failures = m_result->failures();
  JUnitTestResultCollector::TestFailures::const_iterator itFailure = failures.begin();
  while ( itFailure != failures.end() )
  {
    TestFailure *failure = *itFailure++;
    failedTests.insert( std::pair<Test* const, TestFailure*>(failure->failedTest(), failure ) );
  }
}


void
JUnitXmlOutputter::addAllTests( FailedTests &failedTests,
                              XmlElement *rootNode )
{
  const TestResultCollector::Tests &tests = m_result->tests();
  typedef	std::list<Test*>	type_TestList;
  typedef	boost::unordered_map<std::string,type_TestList>	type_testMap;
  
  type_testMap	test_map;
  for ( unsigned int testNumber = 0; testNumber < tests.size(); ++testNumber )
  {
    Test * test = tests[testNumber];
    std::string::size_type n = test->getName().find("::");
		std::string	suite_name;
			if (n >= 0) {
			suite_name = std::string(test->getName(), 0, n);
		}else{
			suite_name = "";
		}
		type_testMap::iterator mit = test_map.find(suite_name);
		if(mit != test_map.end())
		{
			mit->second.push_back(test);
		}else{
			type_TestList	test_list;
			test_list.push_back(test);
			test_map[suite_name] = test_list;
		}
  }
  
  unsigned int idNumber = 0;
  type_testMap::iterator mit = test_map.begin();
  while(mit != test_map.end())
  {
    size_t testCounts = mit->second.size();
    int countErrors = 0;
		type_TestList::iterator lit = mit->second.begin();
		
		XmlElement*	pNode = getTestSuiteNode(rootNode,mit->first,testCounts);
		while(lit != mit->second.end())
		{
			clock_t duration = 0;
			idNumber++;
			if ( failedTests.find( *lit ) == failedTests.end() )
			{
				addSuccessfulTest( *lit, duration, idNumber, pNode );
			}
			else
			{
				addFailedTest( *lit, duration, failedTests[*lit], idNumber, pNode );
				countErrors++;
			}
			lit++;
		}

		char buf[100];
		sprintf(buf, "%d", countErrors);
		pNode->addAttribute("errors", buf);

		sprintf(buf, "%d", countErrors);
		pNode->addAttribute("failures", buf);

		mit++;
	}
		
}


void
JUnitXmlOutputter::addFailedTest( Test * test, 
                                  clock_t duration,
                                  TestFailure *failure,
                                  int testNumber,
                                  XmlElement *rootNode )
{
    XmlElement *testElement = addBaseElement(test, duration, testNumber, rootNode);

    Exception *thrownException = failure->thrownException();

    XmlElement *failureElement = new XmlElement( "failure", thrownException->what() );
    testElement->addElement( failureElement );

    failureElement->addAttribute( "type", failure->isError() ? "error" : "assertion" );
    failureElement->addAttribute( "message", thrownException->message().shortDescription() );
    
    if ( failure->sourceLine().isValid() ) {
        addFailureLocation( failure, failureElement );
    }

    for ( Hooks::iterator it = m_hooks.begin(); it != m_hooks.end(); ++it ) {
        (*it)->successfulTestAdded( m_xml, testElement, test );
    }
}


void
JUnitXmlOutputter::addFailureLocation( TestFailure *failure,
                                       XmlElement *testElement )
{
  XmlElement *locationNode = new XmlElement( "location" );
  testElement->addElement( locationNode );
  SourceLine sourceLine = failure->sourceLine();
  locationNode->addElement( new XmlElement( "file", sourceLine.fileName() ) );
  locationNode->addElement( new XmlElement( "line", sourceLine.lineNumber() ) );
}


void
JUnitXmlOutputter::addSuccessfulTest( Test * test, 
                                      clock_t duration,
                                      int testNumber,
                                      XmlElement *rootNode )
{
    XmlElement *testElement = addBaseElement( test, duration, testNumber, rootNode );
    for ( Hooks::iterator it = m_hooks.begin(); it != m_hooks.end(); ++it ) {
        (*it)->successfulTestAdded( m_xml, testElement, test );
    }
}

XmlElement *
JUnitXmlOutputter::addBaseElement( Test * test, 
                                   clock_t duration,
                                   int testNumber,
                                   XmlElement *rootNode )
{
    XmlElement *testElement = new XmlElement( "testcase" );
    rootNode->addElement( testElement );
    testElement->addAttribute( "id", testNumber );
    std::string::size_type n = test->getName().find("::");
    if (n >= 0) {
        testElement->addAttribute( "class", std::string(test->getName(), 0, n) );
        testElement->addAttribute( "name",  std::string(test->getName(), n+2) );
    }
    else {
        testElement->addAttribute( "class", "" );
        testElement->addAttribute( "name",  test->getName() );
    }

    std::stringstream timing;
    double sec = (double) duration / CLOCKS_PER_SEC;
    timing.precision( 4 );
    timing << std::fixed << sec;

    testElement->addAttribute( "time", timing.str() );
    return testElement;
}


CPPUNIT_NS_END
