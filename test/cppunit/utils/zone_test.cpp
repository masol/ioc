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

#include <cppunit/Test.h>
#include <cppunit/extensions/HelperMacros.h>
#include "utils/zone.h"
#include <stdlib.h>
#include <list>

class ElementInZone : public ioc::ZoneObject
{
public:
	//最小空间要大于Zone增长空间（8K）。否则不会被增加新块。
	static const int buffer_len = 10 * ioc::KB;
	//使用引用检查析构不会被执行。
	bool	&actived;
	//使用这个空间来检查内存是否被正确释放。
	int		buffer[buffer_len];
	ElementInZone(bool &ref) : actived(ref)
	{
		actived = true;
		int i = 0;
		for(int i = 0; i < buffer_len; i++)
		{
			buffer[i] = i;
		}
	}
	
	
	
	~ElementInZone()
	{
		actived = false;
	}
};

template<typename T>
class ZoneList: public std::list<T, ioc::ZoneAllocator<T> > ,
				public ioc::ZoneObject {
public:
	ZoneList() {}
};

struct Element : public ioc::ZoneObject {
	//implement copy constructor and test.
	Element(){
		value = 0;
	}
	Element(const Element& ref){
		value = ref.value;
	}
	int value;
};


class ZoneTest : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(ZoneTest);
		CPPUNIT_TEST(testZoneScope);
		CPPUNIT_TEST(testSTLZone);
	CPPUNIT_TEST_SUITE_END();
public:
	// Set up context before running a test.
	void setUp(void) {
		//ioc::ZoneStorage::setThreadSafe(false);
	}
	// Clean up after the test run.
	void tearDown(void) {} 

protected:
	void testSTLZone(void){
		ioc::ZoneScope	scope_handler;
		
		ZoneList<Element> m_list;
		//ele malloc in stack. this is correct.
		Element ele;
		ele.value = 1234;
		//m_list中的ele在Zone中分配。
		m_list.push_front(ele);
		{
			ioc::ZoneScope	scope_handler;
			CPPUNIT_ASSERT(scope_handler.nestingCount() == 2 && !scope_handler.ShouldDeleteOnExit() && "ZoneScope嵌套计算错误!");
		}
		CPPUNIT_ASSERT(scope_handler.nestingCount() == 1 && scope_handler.ShouldDeleteOnExit() && "ZoneScope嵌套计算错误!");
		Element& refEle = m_list.front();
		CPPUNIT_ASSERT(refEle.value == 1234 && "refEle内存分配错误。");
		CPPUNIT_ASSERT(ioc::ZoneStorage::getStorage().m_zone.isInZone(&refEle) && "refEle内存分配错误。");
		ZoneList<Element>  *plist = new ZoneList<Element>();
		plist->push_front(ele);
		refEle = m_list.front();
		CPPUNIT_ASSERT(refEle.value == 1234 && "refEle内存分配错误。");
		CPPUNIT_ASSERT(ioc::ZoneStorage::getStorage().m_zone.isInZone(&refEle) && "refEle内存分配错误。");
		CPPUNIT_ASSERT(ioc::ZoneStorage::getStorage().m_zone.isInZone(plist) && "refEle内存分配错误。");
	}
	void testZoneScope(void) {
		bool bActived = false;
		bool bSucc1 = false;
		intptr_t	ptr2Offset;
		ElementInZone	*ptr = NULL;
		{
			//将本Scope提到函数级，则内存释放的检查将不会通过。因为就不会释放。
			ioc::ZoneScope	scope_handler;
			bool bHodled = false;
			//ptr2将不会被释放，因为Zone会保留一个块不释放。
			ElementInZone	*ptr2 = new ElementInZone(bHodled);
			ptr2Offset = ioc::OffsetFrom(ptr2);
			ptr2Offset -= (ElementInZone::buffer_len - 1);
			ptr = new ElementInZone(bActived);
			bSucc1 = (bActived == true);
		}
		bool bInRange = false;
		//use malloc to alloc.this will cause heap changed.
		for(int i = 0; i < 10; i++){
			int* s = (int*)malloc(ElementInZone::buffer_len * sizeof(int));
			if( ioc::OffsetFrom(s) >= ptr2Offset && ioc::OffsetFrom(s) < ptr2Offset + ElementInZone::buffer_len)
			{
				bInRange = true;
				for(int j = 0; j < ElementInZone::buffer_len; j++ )
				{
					s[j] = rand();
				}
			}
			free(s);
		}
		bool bFree = false;
		if(bInRange)
		{//only 
		//TODO: we need install handler to catch memory access failed.
#if !defined(IOC_OS_MACOS)
			try{
				for(int i = 0; i < ElementInZone::buffer_len;i++)
				{
					if(ptr->buffer[i] != i)
					{
						bFree = true;
						break;
					}
				}
			}catch(...){
				bFree = true;
			}
#else
			bFree = true;
#endif //IOC_OS_MACOS
		}else{
			bFree = true;
		}
		CPPUNIT_ASSERT(bFree && "Zone中创建的对象没有被正确释放。");
		CPPUNIT_ASSERT((bActived == true) && bSucc1   && "Zone中创建的对象的析构函数被调用。这是错误的。");
	}
};
CPPUNIT_TEST_SUITE_REGISTRATION(ZoneTest);