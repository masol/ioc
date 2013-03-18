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

#include <iostream>
#include <string>
#include <sstream>
#include <cppunit/Test.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/thread/thread.hpp>
#include "utils/log.h"

#if defined(WIN32) || defined(_WIN32) || defined (IOC_OS_WIN)
#include "windows.h"
#else
#include <sys/stat.h>
#endif
/**
 * @brief           test for log
 * @details         测试内容[不断扩充]：
 *                  - log的stream配置
 *                  - log配合boost::log库的普通宏使用
 *                  - Sink类型是TextFile, 文件名没有指定
 *                  - 多个Sink的情况
 *                  - 测试日志的rotation
 *                  - 测试多个sink
 *                  - 测试多个stdout的sink
 *                  - 测试多个同名文件
 **/
class LogTest: public CppUnit::TestFixture
{
        public:
			static std::string tmpPath;
			std::string & getTmpPath()
			{
				if(tmpPath.length()>0)return tmpPath;
#if defined(WIN32) || defined(_WIN32) || defined (IOC_OS_WIN)
 			  char pathname[1024];
			  if (GetTempPathA(1024, pathname)) 
			  {
				  tmpPath=pathname;
				  tmpPath+="ioc\\";
				  ::CreateDirectoryA(tmpPath.c_str(),NULL);
				  return tmpPath;
			  }
			  tmpPath="";
			  return tmpPath;
#else					
		  tmpPath="/tmp/ioc/";
		  mkdir(tmpPath.c_str(), S_IRWXU);
		  return tmpPath;
#endif
			}
        public:
                //{{{对外接口 begin

                /** 
                 * @brief     普通测试
                 **/
                void test_stream_configuration()
                {
					std::string c="Sink.1.Destination=TextFile\n";
					c+=	"Sink.1.FileName=" ;
 
					c+= getTmpPath() ;
					c+= 	"test.log\n";
                    c+="Sink.1.AutoFlush=true\n";
                    c+="Sink.1.RotationSize=1024000\n";
                    c+=" Sink   .  1  .  RotationTime =1 2:3:4\n ";/* 每月1日2点3分4秒 */
                    c+="Sink.1.MinLevel=WARN";
					std::stringstream config(c);
                    ioc::utils::Log::instance().configuration(config);
                    IOC_LOG_SEV(EMERG) << "THIS IS A TEST";
                    IOC_LOG_SEV(INFO) << "THIS IS A TEST";
                    IOC_LOG() << "THIS IS A TEST";
                    IOC_LOG_SEV(_ERROR) << "THIS IS A TEST";
                }

                /**
                 * @brief           测试rotation_time
                 **/
                void test_rotation_time()
                {
                    /* FileName中,%Y-年，%m-月，%d-日, %H-时, %M-分, %S-秒,%N序号 */
                    std::string c("Sink.1.Destination=TextFile\n");
                    c+="Sink.1.FileName=" ;
 
					c += getTmpPath() ;
					c += 	"test_%Y-%m-%d_%H-%M-%S_%N.log\n";
                    c+="Sink.1.AutoFlush=true\n";
                    c+="Sink.1.RotationSize=16\n";
                    c+="Sink.1.RotationTime =2:3:4\n ";/* 每天2点3分4秒 */
                    c+="Sink.1.MinLevel=WARN" ;
					std::stringstream config(c);
                    ioc::utils::Log::instance().configuration(config);
                    IOC_LOG_SEV(EMERG) << "THIS IS A TEST";
                    IOC_LOG_SEV(INFO) << "THIS IS A TEST";
                    IOC_LOG() << "THIS IS A TEST";
                    IOC_LOG_SEV(_ERROR) << "THIS IS A TEST";
                }
                
                
                /** 
                 * @brief           测试日志的rotation
                 * @expect          会分裂成多个日志文件
                 **/
                void test_rotation()
                {
                    std::string c("Sink.1.Destination=TextFile\n");
                    c+="Sink.1.FileName=" ;
 
					c+= getTmpPath() ;
					c+="rotation_%N.log\n";
                    c+="Sink.1.AutoFlush=true\n";
                    c+="Sink.1.RotationSize=16\n";
                   c+= "Sink.1.MinLevel=NORM"                     ;
					std::stringstream config(c);
                    ioc::utils::Log::instance().configuration(config);
                    IOC_LOG_SEV(EMERG) << "THIS IS A TEST";
                    IOC_LOG_SEV(INFO) << "THIS IS A TEST";
                    IOC_LOG_SEV(_ERROR) << "THIS IS A TEST";
                }
                
                /** 
                 * @brief       测试多个sink
                 * @expect      每个目标有自己独立的一份日志副本
                 **/
                void test_mutisink()
                {
                    std::string c("Sink.1.Destination=TextFile\n");
                    c+="Sink.1.FileName=" ;
 
					c +=getTmpPath() ;
					c +="muti1_%N.log\n";
                    c+="Sink.1.AutoFlush=true\n";
                    c+="Sink.1.RotationSize=16\n";
                    c+="Sink.1.MinLevel=NORM\n";
                    c+="Sink.3.Destination=TextFile\n";
                    c+="Sink.3.FileName=" ;
 
					c+=getTmpPath() ;
					c+= 	"muti2_%N.log\n";
                    c+="Sink.3.AutoFlush=true\n";
                    c+="Sink.3.RotationSize=16\n";
                    c+="Sink.3.MinLevel=EMERG\n";
                    c+="Sink.4.Destination=Console\n";
                    c+="Sink.4.MinLevel=EMERG\n";
                    c+="Sink.4.Stream=stdout\n";
                   
					std::stringstream config(c);
                    ioc::utils::Log::instance().configuration(config);
                    IOC_LOG_SEV(EMERG) << "THIS IS A TEST";
                    IOC_LOG_SEV(INFO) << "THIS IS A TEST";
                    IOC_LOG_SEV(_ERROR) << "THIS IS A TEST";
                }
                
                /** 
                 * @brief           测试多个stdout的sink
                 * @expect          会往标准输出打印多个日志信息
                 **/
                void test_mutistdout()
                {
                    std::string c("Sink.2.Destination=Console\n");
                    c+="Sink.2.MinLevel=NORM\n";
                    c+="Sink.2.Stream=stdout\n";
                    c+="Sink.3.Destination=Console\n";
                    c+="Sink.3.MinLevel=ERROR\n";
                    c+="Sink.3.Stream=stdout\n";
                  
					std::stringstream config(c);
                    ioc::utils::Log::instance().configuration(config);
                    IOC_LOG_SEV(EMERG) <<__FUNCTION__<< ":THIS IS A TEST";
                    IOC_LOG_SEV(INFO) <<__FUNCTION__<< ":THIS IS A TEST";
                    IOC_LOG_SEV(_ERROR)<<__FUNCTION__ << ":THIS IS A TEST";
                }

                /** 
                 * @brief           测试多个同名文件
                 * @expect          一个文件收到多个日志副本
                 **/
                void test_mutifile()
                {
                    std::string c("Sink.1.Destination=TextFile\n");
                    c+="Sink.1.FileName=";
 
					c += getTmpPath() ;
					c += 	"test.log\n";
                    c+="Sink.1.AutoFlush=true\n";
                    c+="Sink.1.RotationSize=1024000\n";
                    c+="Sink.1.MinLevel=WARN\n";
                    c+="Sink.2.Destination=TextFile\n";
                   c+= "Sink.2.FileName=test.log\n";
                   c+= "Sink.2.AutoFlush=true\n";
                   c+= "Sink.2.RotationSize=1024000\n";
                   c+= "Sink.2.MinLevel=WARN\n";
                      
					std::stringstream config(c);
                    ioc::utils::Log::instance().configuration(config);
                    IOC_LOG_SEV(EMERG) <<__FUNCTION__<< ":THIS IS A TEST";
                    IOC_LOG_SEV(INFO) <<__FUNCTION__<< ":THIS IS A TEST";
                    IOC_LOG_SEV(_ERROR)<<__FUNCTION__ << ":THIS IS A TEST";
                }
    
    
                /** 
                 * @brief           初始化(每个测试函数开始时都会调用该函数)
                 * @return
                 **/
                void setUp()
                {
                }

                /**
                 * @brief           结束(每个测试函数结束时都会调用这个函数)
                 **/
                void tearDown()
                {
                }


                CPPUNIT_TEST_SUITE(LogTest);
                CPPUNIT_TEST(test_stream_configuration);
                CPPUNIT_TEST(test_rotation);
                CPPUNIT_TEST(test_mutisink);
                CPPUNIT_TEST(test_mutistdout);
                CPPUNIT_TEST(test_mutifile);
                CPPUNIT_TEST(test_rotation_time);
                CPPUNIT_TEST_SUITE_END();
                
                

                //}}}对外接口 end


        protected:
                //{{{保护成员 begin

                //}}}测试用例
                
                //}}}保护成员 end


        private:
                //{{{私有成员 begin
                //}}}私有成员 end


}; /* -----  end of class OptionTest  ----- */
 std::string LogTest::tmpPath;

CPPUNIT_TEST_SUITE_REGISTRATION(LogTest);
