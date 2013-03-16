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

#include <cppunit/TestFailure.h>
#include "JUnitTestResultCollector.h"

#include <algorithm>

CPPUNIT_NS_BEGIN


JUnitTestResultCollector::JUnitTestResultCollector( SynchronizationObject *syncObject )
    : TestResultCollector( syncObject )
{
    reset();
}

JUnitTestResultCollector::~JUnitTestResultCollector()
{
}

void 
JUnitTestResultCollector::reset()
{
    TestResultCollector::reset();

    ExclusiveZone zone( m_syncObject ); 
    m_timings.clear();
    m_startAll = 0;
    m_durationAll = 0;
}

void JUnitTestResultCollector::startTestRun(Test * /*test*/, TestResult * /*eventManager*/ ) 
{
    m_startAll = clock();
}

void 
JUnitTestResultCollector::startTest( Test *test )
{
    TestResultCollector::startTest( test );

    ExclusiveZone zone (m_syncObject); 
    m_timings.push_back( test );
}

void 
JUnitTestResultCollector::endTest( Test *test )
{
    TestResultCollector::endTest( test );

    ExclusiveZone zone (m_syncObject); 
    TimingResults::iterator i = std::find(m_timings.begin(), m_timings.end(), test);
    if (i != m_timings.end()) {
        i->duration = clock() - i->start;
    }
}

void JUnitTestResultCollector::endTestRun(Test * /*test*/, TestResult * /*eventManager*/ ) 
{
    m_durationAll = clock() - m_startAll;
}

clock_t 
JUnitTestResultCollector::getTestDuration( Test * test )
{
    ExclusiveZone zone (m_syncObject); 
  
    TimingResults::iterator i = std::find(m_timings.begin(), m_timings.end(), test);
    if (i != m_timings.end()) {
        return i->duration;
    }
    return 0;
}

CPPUNIT_NS_END

