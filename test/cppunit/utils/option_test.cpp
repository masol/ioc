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
#include <cppunit/Test.h>
#include <boost/thread/thread.hpp>
#include "utils/option.h"
#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief           test for Option
 * @details         测试内容[不断扩充]：
 *                  OPtion基本的功能[OK]
 *                  线程安全测试
 *                  在监视函数里面调用Option的成员，测试可能的死锁[OK]
 *                  测试链式监视，在监视函数里面调用老的监视函数[OK]
 *                  测试erase一个不存在的节点，不抛异常[OK]
 *                  测试在监视函数中抛异常[OK]
 *                  测试监测函数return false后，操作是否进行[OK]
 **/
class OptionTest: public CppUnit::TestFixture
{
        public:
                //{{{结构和类型定义 begin
                typedef ioc::utils::Option::slottype slottype;
                typedef ioc::utils::Option::Act_Type Act_Type;
                typedef boost::optional<slottype>   OPT_SLOT;
                typedef ioc::utils::Option OP;
                //}}}结构和类型定义 end


        public:
                //{{{对外接口 begin


                /** 
                 * @brief           test 
                 * template<typename Type>
                 * Type get(const std::string& path) const
                 **/
                void test_get_s()
                {
                    OP::instance().put(property_1(), i_1());
                    CPPUNIT_ASSERT( OP::instance().get<int>(property_1()) == i_1() );

                    OP::instance().put(property_1_2(), str_1_2());
                    CPPUNIT_ASSERT( OP::instance().get<std::string>(property_1_2()) == str_1_2() );

                    /* 测试属性不存在时抛异常 */
                    CPPUNIT_ASSERT_THROW(OP::instance().get<std::string>(property_not_existed()),  boost::property_tree::ptree_bad_path);

                    /* 测试类型转化失败抛异常 */
                    CPPUNIT_ASSERT_THROW( OP::instance().get<int>(property_1_2()), boost::bad_any_cast);

                }
    
    
    
                /** 
                 * @brief           test 
                 * template<typename Type>
                 * boost::optional<Type> get_optional(const std::string& path) const
                 **/
                void test_get_optional_s()
                {
                    OP::instance().put(property_1(), i_1());
                    boost::optional<int> iopt = OP::instance().get_optional<int>(property_1());
                    CPPUNIT_ASSERT(iopt.is_initialized());
                    CPPUNIT_ASSERT(*iopt == i_1());

                    OP::instance().put(property_2(), str_2());
                    boost::optional<std::string> stropt = OP::instance().get_optional<std::string>(property_2());

                    CPPUNIT_ASSERT(stropt.is_initialized());
                    CPPUNIT_ASSERT(*stropt == str_2());

                    /* 测试一个不存在的属性名 */
                    boost::optional<std::string> strvalue =OP::instance().get_optional<std::string>(property_not_existed());
                    CPPUNIT_ASSERT( !strvalue.is_initialized() );

                    /* 测试类型转化失败 */
                    OP::instance().put(property_1_2(), str_1_2());
                    boost::optional<int> ivalue =OP::instance().get_optional<int>(property_1_2());
                    CPPUNIT_ASSERT( !ivalue.is_initialized() );

                }
    
    
                /** 
                 * @brief           test 
                 * bool connect(const std::string& path, slottype s)
                 **/
                void test_connect_s_s()
                {
                    OP::instance().put(property_1(), i_1());

                    OPT_SLOT p = OP::instance().connect(property_1(), slot_throw<int>);
                    /* 测试对一个没有监视函数的属性监视，
                     * 返回一个没有初始化的opt */
                    CPPUNIT_ASSERT(!p.is_initialized());

                    OPT_SLOT q = OP::instance().connect(property_1(), slot_throw<char>);
                    CPPUNIT_ASSERT(q.is_initialized());

                    /* assert 调用的是slot_throw<int> */
                    std::string str;
                    boost::any any = 0;
                    Act_Type a = ioc::utils::Option::ACT_NULL;
                    CPPUNIT_ASSERT_THROW((*q)(str, any, a), int);

                    /* put的时候，slot_throw<char>被调用 */
                    CPPUNIT_ASSERT_THROW( OP::instance().put(property_1(), i_1()), char);
                    
                    OP::instance().put(property_2(), str_2() );
                    OP::instance().connect(property_2(), slot_return_false);
                    OP::instance().put(property_2(), str_1_2() );

                    /* 断言property_2的值没有改变 */
                    CPPUNIT_ASSERT(OP::instance().get<std::string>( property_2() ) == str_2() );

                    OP::instance().connect(property_2(), slot_return_true);
                    OP::instance().put(property_2(), str_1_2());
                    /* 断言property_2的值已经变成str_1_2了 */
                    CPPUNIT_ASSERT(OP::instance().get<std::string>( property_2() ) == str_1_2() );


                }

                /**
                 * @brief           connect返回回调函数的调用, 链式调用
                 **/
                void test_connect_link()
                {
                    OP::instance().put(property_1(), i_1());
                    OP::instance().connect(property_1(), slot_throw<int>);
                    old_slot = OP::instance().connect(property_1(), slot_call_old);

                    //断言slot_throw会被执行
                    CPPUNIT_ASSERT_THROW(OP::instance().put(property_1(), i_1()), int);

                }
    
                /**
                 * @brief           测试线程安全
                 **/
                void test_muti_thread()
                {
                    // boost::thread_group threads;
                    // for (int i = 0; i < 10; ++i)
                      // threads.create_thread();
                    // threads.join_all();
                }
    
                /** 
                 * @brief           test 
                 * bool disconnect(const std::string& path, slottype s)
                 **/
                void test_disconnect_s_s()
                {
                    OP::instance().put(property_1(), i_1());
                    OP::instance().connect(property_1(), slot_throw<int>);
                    CPPUNIT_ASSERT_THROW(OP::instance().put(property_1(), i_1()), int);
                    OP::instance().disconnect(property_1());
                    CPPUNIT_ASSERT_NO_THROW(OP::instance().put(property_1(), i_1()));
                }
    
    
                /** 
                 * @brief           test 
                 * template<typename Type>
                 * void put(const std::string& path, const Type & value)
                 **/
                void test_put_s_t()
                {
                    /* 上面已经测试 */
                }
    
    
                /** 
                 * @brief           test
                 * bool is_existed(const std::string & path)
                 **/
                void test_is_existed()
                {
                    OP::instance().put(property_1(), i_1());
                    CPPUNIT_ASSERT( OP::instance().get<int>(property_1()) == i_1() );
                    CPPUNIT_ASSERT(OP::instance().is_existed(property_1()));
                    CPPUNIT_ASSERT(!OP::instance().is_existed(property_not_existed()));
                }


                
                /** 
                 * @brief           test
                 * void erase(const std::string & path)
                 **/
                void test_erase()
                {
                    OP::instance().put(property_1(), i_1());
                    CPPUNIT_ASSERT_NO_THROW(OP::instance().get<int>(property_1()));
                    OP::instance().erase( property_1() );
                    CPPUNIT_ASSERT_THROW(OP::instance().get<int>(property_1()), boost::property_tree::ptree_bad_path);

                    /* 测试删除一个不存在的节点 */
                    CPPUNIT_ASSERT_NO_THROW(OP::instance().erase(  property_not_existed() ) );
                    return ;
                }       /* -----  end of function test_erase  ----- */


                /** 
                 * @brief           测试在监视函数里面调用optional
                 **/
                void test_call_op()
                {
                    OP::instance().put(property_1(), i_1());
                    OP::instance().connect(property_1(), slot_call_op);
                    OP::instance().put(property_1(), i_1());//slot_call_op调用connect(property_1(), slot_throw<int>)
                    CPPUNIT_ASSERT_THROW(OP::instance().put(property_1(), i_1()), int);
                }

				/**
				 * @brief			测试get的boost::any类型特化版
				 **/
				void test_get_any()
				{
					boost::any any = i_1();
					OP::instance().put(property_1(), any);
					CPPUNIT_ASSERT_MESSAGE("以boost::any方式调用get失败",
						boost::any_cast<int>(OP::instance().get<boost::any>(property_1())) == i_1() );
					CPPUNIT_ASSERT_MESSAGE("以boost::any方式调用get失败",
						OP::instance().get<int>(property_1()) == i_1() );
				}

                /** 
                 * @brief           初始化(每个测试函数开始时都会调用该函数)
                 * @return
                 **/
                void setUp()
                {
                    OP::instance().erase(property_1());
                    OP::instance().erase(property_2());
                    OP::instance().erase(property_1_2());
                    OP::instance().erase(property_not_existed());
                    old_slot.reset();
                }

                /**
                 * @brief           结束(每个测试函数结束时都会调用这个函数)
                 **/
                void tearDown()
                {
                    OP::instance().disconnect(property_1());
                    OP::instance().disconnect(property_2());
                    OP::instance().disconnect(property_1_2());
                    OP::instance().disconnect(property_not_existed());
                }


                CPPUNIT_TEST_SUITE(OptionTest);
                CPPUNIT_TEST(test_get_s);
                CPPUNIT_TEST(test_get_optional_s);
                CPPUNIT_TEST(test_connect_s_s);
                CPPUNIT_TEST(test_disconnect_s_s);
                CPPUNIT_TEST(test_put_s_t);
                CPPUNIT_TEST(test_is_existed);
                CPPUNIT_TEST(test_erase);
                CPPUNIT_TEST(test_connect_link);
                CPPUNIT_TEST(test_muti_thread);
                CPPUNIT_TEST(test_call_op);
				CPPUNIT_TEST(test_get_any);
                CPPUNIT_TEST_SUITE_END();
                
                

                //}}}对外接口 end


        protected:
                //{{{保护成员 begin

                //{{{测试用例
                static const std::string property_1(){return "p1";}
                static const std::string property_2(){return "p2";}
                static const std::string property_1_2(){return "p1.p2";}
                static const std::string property_not_existed(){return "not_existed";}
                static const int i_1(){return 1;}
                static const std::string str_2(){return "2";}
                static const std::string str_1_2(){return "1_2";}

                static boost::optional<slottype> old_slot;

                /* 测试用例，回调函数 */
                template <typename T>
                static bool slot_throw(const std::string & str, boost::any & a, Act_Type t)
                {
                    throw T();
                    return true;
                }

                static bool slot_no_throw(const std::string & str, boost::any & a, Act_Type t)
                {
                    return true;
                }

                static bool slot_return_true(const std::string & str, boost::any & a, Act_Type t)
                {
                    return true;
                }

                static bool slot_return_false(const std::string & str, boost::any & a, Act_Type t)
                {
                    return false;
                }

                static bool slot_call_old(const std::string & str, boost::any & a, Act_Type t)
                {
                    if(old_slot.is_initialized())
                    {
                        return (*old_slot)(str, a, t);
                    }
					return true;
                }
                static bool slot_call_op(const std::string & str, boost::any & a, Act_Type t)
                {
                    OP::instance().connect(property_1(), slot_throw<int>);
                    return true;
                }

                //}}}测试用例
                
                //}}}保护成员 end


        private:
                //{{{私有成员 begin
                //}}}私有成员 end


}; /* -----  end of class OptionTest  ----- */

boost::optional<OptionTest::slottype> OptionTest::old_slot;

CPPUNIT_TEST_SUITE_REGISTRATION(OptionTest);
