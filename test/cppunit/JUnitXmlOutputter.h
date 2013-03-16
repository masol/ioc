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

#ifndef IOC_TEST_JUNITXMLTESTRESULTOUTPUTTER_H
#define IOC_TEST_JUNITXMLTESTRESULTOUTPUTTER_H

#include <cppunit/Portability.h>

#if CPPUNIT_NEED_DLL_DECL
#pragma warning( push )
#pragma warning( disable: 4251 )  // X needs to have dll-interface to be used by clients of class Z
#endif

#include <cppunit/Outputter.h>
#include <cppunit/portability/CppUnitDeque.h>
#include <cppunit/portability/CppUnitMap.h>
#include <cppunit/portability/Stream.h>
#include "JUnitTestResultCollector.h"


CPPUNIT_NS_BEGIN


class Test;
class TestFailure;
class XmlDocument;
class XmlElement;
class XmlOutputterHook;


/*! \brief Outputs a TestResultCollector in JUnit XML format.
 * \ingroup WritingTestResult
 *
 * Save the test result as a XML stream in the JUnit format. 
 *
 * Additional datas can be added to the XML document using XmlOutputterHook. 
 * Hook are not owned by the JUnitXmlOutputter. They should be valid until 
 * destruction of the JUnitXmlOutputter. They can be removed with removeHook().
 *
 * \see XmlDocument, XmlElement, XmlOutputterHook.
 */
class CPPUNIT_API JUnitXmlOutputter : public Outputter
{
public:
  /*! \brief Constructs a JUnitXmlOutputter object.
   * \param result Result of the test run.
   * \param stream Stream used to output the XML output.
   * \param encoding Encoding used in the XML file (default is UTF-8). 
   */
  JUnitXmlOutputter( JUnitTestResultCollector *result,
                OStream &stream,
                std::string encoding = std::string("UTF-8") );

  /// Destructor.
  virtual ~JUnitXmlOutputter();

  /*! \brief Adds the specified hook to the outputter.
   * \param hook Hook to add. Must not be \c NULL.
   */
  virtual void addHook( XmlOutputterHook *hook );

  /*! \brief Removes the specified hook from the outputter.
   * \param hook Hook to remove.
   */
  virtual void removeHook( XmlOutputterHook *hook );

  /*! \brief Writes the specified result as an XML document to the stream.
   *
   * Refer to examples/cppunittest/XmlOutputterTest.cpp for example
   * of use and XML document structure.
   */
  virtual void write();

  /*! \brief Sets the XSL style sheet used.
   *
   * \param styleSheet Name of the style sheet used. If empty, then no style sheet
   *                   is used (default).
   */
  virtual void setStyleSheet( const std::string &styleSheet );

  virtual void setHostname( const std::string &hostname ) {
      m_hostname = hostname;
  }

  virtual void setName( const std::string &name ) {
      m_name = name;
  }

  /*! \brief set the output document as standalone or not.
   *
   *  For the output document, specify wether it's a standalone XML
   *  document, or not.
   *
   *  \param standalone if true, the output will be specified as standalone.
   *         if false, it will be not.
   */
  virtual void setStandalone( bool standalone );

  typedef CppUnitMap<Test *,TestFailure*, std::less<Test*> > FailedTests;

  /*! \brief Sets the root element and adds its children.
   *
   * Set the root element of the XML Document and add its child elements.
   *
   * For all hooks, call beginDocument() just after creating the root element (it
   * is empty at this time), and endDocument() once all the datas have been added
   * to the root element.
   */
  virtual void setRootNode();
  XmlElement*  getTestSuiteNode(XmlElement *rootNode,const std::string& name,size_t testCounts);

  virtual void addAllTests( FailedTests &failedTests,
                            XmlElement *rootNode );

  /*! \brief Adds a failed test to the failed tests node.
   * Creates a new element containing datas about the failed test, and adds it to 
   * the failed tests element.
   * Then, for all hooks, call failTestAdded().
   */
  virtual void addFailedTest( Test * test, 
                              clock_t duration,
                              TestFailure *failure,
                              int testNumber,
                              XmlElement *rootNode );

  virtual void addFailureLocation( TestFailure *failure,
                                   XmlElement *testElement );


  /*! \brief Adds a successful test to the successful tests node.
   * Creates a new element containing datas about the successful test, and adds it to 
   * the successful tests element.
   * Then, for all hooks, call successfulTestAdded().
   */
  virtual void addSuccessfulTest( Test * test, 
                                  clock_t duration,
                                  int testNumber,
                                  XmlElement *rootNode );
protected:
  virtual void fillFailedTestsMap( FailedTests &failedTests );
  virtual XmlElement * addBaseElement( Test * test, clock_t duration, int testNumber, XmlElement *rootNode );

protected:
  typedef CppUnitDeque<XmlOutputterHook *> Hooks;

  JUnitTestResultCollector *m_result;
  OStream &m_stream;
  std::string m_encoding;
  std::string m_styleSheet;
  XmlDocument *m_xml;
  Hooks m_hooks;
  std::string m_hostname;
  std::string m_name;

private:
  /// Prevents the use of the copy constructor.
  JUnitXmlOutputter( const JUnitXmlOutputter &copy );

  /// Prevents the use of the copy operator.
  void operator =( const JUnitXmlOutputter &copy );

private:
};


CPPUNIT_NS_END

#if CPPUNIT_NEED_DLL_DECL
#pragma warning( pop )
#endif


#endif  //IOC_TEST_JUNITXMLTESTRESULTOUTPUTTER_H
