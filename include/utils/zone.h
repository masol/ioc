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

/**
 * @file        zone.h
 * @brief       zone提供了小块内存的快速存取。这些小块内存不能被单独删除。
 * 				但是zone提供了整块内存的快速释放。这可以为临时内存提供支持，
 * 				例如AST变幻，在变幻完成时释放占据内存。
 **/


#ifndef  IOC_UTILS_ZONE_H
#define  IOC_UTILS_ZONE_H

#include "utils/macro.h"
#include "utils/constants.h"
#include <boost/limits.hpp>
#include <list>
#include <vector>

#ifdef max
#undef max
#endif //max

#ifdef min
#undef min
#endif //min

namespace ioc {

    /** @defgroup zone 类GC内存管理
	 * @ingroup utils
     * @brief   zone提供了小块内存的快速存取。
	 * @details  这些小块内存不能被单独删除。但是zone提供了整块内存的快速释放。这可以为临时内存提供支持，
	 * 				例如AST变幻，在变幻完成时释放占据内存。
     * 使用举例:
		 * @code
				ioc::ZoneScope	scope_handler;
				use zoneObject....
		 * @endcode
		 * @see ZoneTest
		 * @{
		 */
	
/** @brief 鉴于AST接口的复杂度，zone还是取消了模版实现。转而使用线程不安全和线程安全两个版本。
 * @details 线程安全版本使用TLS来存储线程变量，因此，每个线程内的分配和销毁都是安全的，但是分配的空间跨线程之后就不保障了。
 * 			而线程不安全版本使用了一个全局zone,因此分配和销毁在跨越线程时不能保障安全性。
 * 			经过这次refine,zone接口更像一个GC接口了——只不过不如GC那样面向函数，而是面向线程的。
 **/
 
class Zone {
public:
	/// @brief 分配大小为‘size'的内存块，如果有必要，会扩展zone.
	
	inline void* New(int size) {
		// Round up the requested size to fit the alignment.
		size = RoundUp(size, kAlignment);
		// Check if the requested size is available without expanding.
		Address result = position_;
		if ((position_ += size) > limit_) result = NewExpand(size);
		// Check that the result has the proper alignment and return it.
		IOC_ASSERT(IsAddressAligned(result, kAlignment, 0));
		return reinterpret_cast<void*>(result);
	}

	template <typename T>
	inline T* NewArray(int length) {
		return static_cast<T*>(Zone::New(length * sizeof(T)));
	}

	/// @brief 销毁Zone中已分配的内存，这将导致所有由Zone分配的内存不能继续使用。
	
	void DeleteAll();

	/// @brief 如果已分配内存超过了Zone的内存界限，返回true。
	
	inline bool excess_allocation() {
		return segment_bytes_allocated_ > zone_excess_limit_;
	}
	inline void adjust_segment_bytes_allocated(int delta) {
		segment_bytes_allocated_ += delta;
	}
	bool	isInZone(void* ptr);
private:
	friend class ZoneStorage;
	Zone() :zone_excess_limit_(0),segment_bytes_allocated_(0),position_(0),limit_(0) {
	}
	~Zone() {
		IOC_UNREACHABLE();
	}
	/// @brief 所有分配的内存块是kAlignment对齐的。
	
	static const int kAlignment = kPointerSize;
	
	/// @brief 每次扩展最小分配单元。
	
	static const int kMinimumSegmentSize = 8 * KB;
	
	/// @brief 每次扩展最大分配单元。
	
	static const int kMaximumSegmentSize = 1 * MB;
	static const int kMaximumKeptSegmentSize = 64 * KB;
	int zone_excess_limit_;
	int segment_bytes_allocated_;
	Address NewExpand(int size);
	Address position_;
	Address limit_;
};


class ZoneStorage {
private:
	int			m_depth;
	friend class ZoneScope;
	ZoneStorage() : m_depth(0) {}
public:
	Zone		m_zone;
	~ZoneStorage() {
		//m_zone.DeleteAll();
		IOC_UNREACHABLE();
	}
	static ZoneStorage& getStorage(void);
#ifdef IOC_NOTHREAD_SAFE
	static inline bool		isThreadSafe(void){
		return false;
	}
#else
	static inline bool		isThreadSafe(void){
		return true;
	}
#endif
};

class ZoneScope {
	public:

	explicit ZoneScope() {
		ZoneStorage::getStorage().m_depth++;
	}
	
	virtual ~ZoneScope() {
		if(ShouldDeleteOnExit()){
			ZoneStorage::getStorage().m_zone.DeleteAll();	
		}
		ZoneStorage::getStorage().m_depth--;
	}
	
	bool ShouldDeleteOnExit() {
		return ZoneStorage::getStorage().m_depth == 1;
	}
	///@brief 返回嵌套深度。
	
	static int nestingCount()
	{	return ZoneStorage::getStorage().m_depth; }

};

class ZoneObject {
public:
	// Allocate a new ZoneObject of 'size' bytes in the Zone.
	inline void* operator new(size_t size) throw() 
	{ return ZoneStorage::getStorage().m_zone.New(static_cast<int>(size)); }
	inline void* operator new(size_t size, void *ptr) throw() 
	{ return ptr;}
	inline void* operator new[](std::size_t size) throw() 
	{ return ZoneStorage::getStorage().m_zone.New(static_cast<int>(size)); }
	inline void* operator new[](std::size_t, void* __p) throw() 
	{ return __p; }
	

	void operator delete(void*, void*) {
		IOC_UNREACHABLE();
	}
	void operator delete(void*, size_t) {
		IOC_UNREACHABLE();
	}
};

/// @brief ZoneListAllocationPolicy作为adapter,可以将STL与Zone联合使用。

template<typename T>
struct ZoneAllocator {
public:
	typedef T 					value_type;
	typedef value_type* 		pointer;
	typedef const value_type* 	const_pointer;
	typedef value_type& 		reference;
	typedef const value_type& 	const_reference;
	typedef std::size_t 		size_type;
	typedef std::ptrdiff_t 		difference_type;

public:
	template<typename U>
	struct rebind {
		typedef ZoneAllocator<U> other;
	};
public:
	inline explicit ZoneAllocator() {}
	inline ~ZoneAllocator() {}
	inline explicit ZoneAllocator(ZoneAllocator const&) {}
	
	template <typename U>
	inline ZoneAllocator(ZoneAllocator<U> const&) {}

    static pointer allocate(const size_type n) {
		return reinterpret_cast<pointer>(ZoneStorage::getStorage().m_zone.New( (int)(n * sizeof (T))) );
     }

	inline pointer allocate(size_type n,const void * const) {
		return allocate(n);
	}

	///按照Zone定义，释放行为不会发生。

	inline void deallocate(pointer p, size_type)  {
	}

	inline size_type max_size() const {
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	inline pointer address(reference r) {
		return &r;
	}

	inline const_pointer address(const_reference r) {
		return &r;
	}


	inline void construct(pointer p, const T& t) {
		new(p) T(t);
	}
	
	///FIXME:按照Zone定义，释放行为不会发生。
	
	inline void destroy(pointer p) {
		p->~T();
	}

	inline bool operator==(ZoneAllocator const&) {
		return true;
	}
	inline bool operator!=(ZoneAllocator const& a) {
		return !operator==(a);
	}
};


template<typename T>
class ZoneList: public std::list<T, ioc::ZoneAllocator<T> > ,
				public ioc::ZoneObject {
public:
	ZoneList() {}
};

template<typename T>
class ZoneVector: public std::vector<T, ioc::ZoneAllocator<T> > ,
				public ioc::ZoneObject {
public:
	ZoneVector() {}
};

/**@}*/ //end group of zone

}


#endif //IOC_UTILS_ZONE_H
