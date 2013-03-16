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

#ifndef IOC_TEST_JUNITTESTRESULTCOLLECTOR_H
#define IOC_TEST_JUNITTESTRESULTCOLLECTOR_H

#include <time.h>
#include <cppunit/Portability.h>

#if CPPUNIT_NEED_DLL_DECL
#pragma warning( push )
#pragma warning( disable: 4251 4660 )  // X needs to have dll-interface to be used by clients of class Z
#endif

#include <cppunit/TestResultCollector.h>
#include <cppunit/TestSuccessListener.h>
#include <cppunit/portability/CppUnitDeque.h>


CPPUNIT_NS_BEGIN

#if CPPUNIT_NEED_DLL_DECL
//  template class CPPUNIT_API std::deque<TestFailure *>;
//  template class CPPUNIT_API std::deque<Test *>;
#endif


/*! \brief Collects test result.
 * \ingroup WritingTestResult
 * \ingroup BrowsingCollectedTestResult
 * 
 * A JUnitTestResultCollector is a TestListener which collects the results of executing 
 * a test case. It is an instance of the Collecting Parameter pattern.
 *
 * The test framework distinguishes between failures and errors.
 * A failure is anticipated and checked for with assertions. Errors are
 * unanticipated problems signified by exceptions that are not generated
 * by the framework.
 * \see TestListener, TestFailure.
 */
class CPPUNIT_API JUnitTestResultCollector : public TestResultCollector
{
public:
    struct TimingResult {
        Test *  test;
        clock_t start;
        clock_t duration;

        TimingResult(Test * aTest) : test(aTest) {
            start = clock();
            duration = 0;
        }
        TimingResult(const TimingResult& rhs) : test(rhs.test), start(rhs.start), duration(rhs.duration) { }
        bool operator==(const TimingResult& rhs) { return test == rhs.test; }
    };
    typedef CppUnitDeque<TimingResult> TimingResults;


    /*! Constructs a JUnitTestResultCollector object.
    */
    JUnitTestResultCollector( SynchronizationObject *syncObject = 0 );

    /// Destructor.
    virtual ~JUnitTestResultCollector();

    virtual void startTestRun(Test * test, TestResult * eventManager );
    virtual void startTest( Test *test );
    virtual void endTest( Test *test );
    virtual void endTestRun(Test * test, TestResult * eventManager );
    virtual void reset();

    clock_t getTestDuration( Test * test );
    clock_t getAllTestsDuration() const { return m_durationAll; }

protected:
    TimingResults   m_timings;
    clock_t         m_startAll;
    clock_t         m_durationAll;

private:
  /// Prevents the use of the copy constructor.
  JUnitTestResultCollector( const JUnitTestResultCollector &copy );

  /// Prevents the use of the copy operator.
  void operator =( const JUnitTestResultCollector &copy );
};



CPPUNIT_NS_END

#if CPPUNIT_NEED_DLL_DECL
#pragma warning( pop )
#endif


#endif  //IOC_TEST_JUNITTESTRESULTCOLLECTOR_H
