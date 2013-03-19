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
 * @file        macor.h
 * @brief       macro提供了ioc常用宏。支持了如下方法
 * 				- IOC_ASSERT
 * 				- IOC_FATAL / IOC_UNIMPLEMENTED / IOC_UNREACHABLE
 **/

#ifndef  IOC_UTILS_MACRO_H
#define  IOC_UTILS_MACRO_H

/*! \defgroup utils 通用工具
 *  ioc支持如下工具类:
 *  @{
 */


#ifndef _DEBUG
#ifndef BOOST_DISABLE_ASSERTS
#define BOOST_DISABLE_ASSERTS 1
#endif
#endif
#include <boost/assert.hpp>

/*! \defgroup assert 断言支持
 *  @{
 */

#ifdef _DEBUG
#define IOC_ASSERT(expr) BOOST_ASSERT(expr)
#else
#define IOC_ASSERT(expr) ((void) 0)
#endif

#ifdef _DEBUG
#define IOC_FATAL(msg)                              \
	IOC_ASSERT(false && (msg) && __FILE__ && __LINE__ )
#define IOC_UNIMPLEMENTED()                         \
	IOC_ASSERT(false && ("unimplemented code") && __FILE__ && __LINE__ )
#define IOC_UNREACHABLE()                           \
	IOC_ASSERT(false && ("unreachable code") && __FILE__ && __LINE__ )
#else
#define IOC_FATAL(msg)  ((void) 0)
#define IOC_UNIMPLEMENTED() ((void) 0)
#define IOC_UNREACHABLE()  ((void) 0)
#endif

/** @} */ //end of assert

#include <stdio.h>

#if defined(_WIN32) && !defined(__MINGW32__)

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;  // NOLINT
typedef unsigned short uint16_t;  // NOLINT
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
// intptr_t and friends are defined in crtdefs.h through stdio.h.

#else

#include <stdint.h>

#endif

namespace ioc {
typedef uint8_t byte;
typedef byte* Address;

/** @defgroup basicMath 语言相关数学计算
 *  @ingroup utils
 *  @{
 */

/// @brief 判定x是否是2的n次冥
#define IS_POWER_OF_TWO(x) (((x) & ((x) - 1)) == 0)

/// @brief  如果x是2的n次冥或者0，则返回true.否则返回false.不能用于判定类型T的最大负值(-1)
template <typename T>
static inline bool IsPowerOf2(T x) {
	return IS_POWER_OF_TWO(x);
}

/// @brief x是2的n次冥，返回n。x必须是2的n次冥.
template <typename T>
static inline int WhichPowerOf2(T x) {
	ASSERT(IsPowerOf2(x));
	ASSERT(x != 0);
	if (x < 0) return 31;
	int bits = 0;
#ifdef DEBUG
	int original_x = x;
#endif
	if (x >= 0x10000) {
		bits += 16;
		x >>= 16;
	}
	if (x >= 0x100) {
		bits += 8;
		x >>= 8;
	}
	if (x >= 0x10) {
		bits += 4;
		x >>= 4;
	}
	switch (x) {
	default:
		IOC_UNREACHABLE();
	case 8:
		bits++;  // Fall through.
	case 4:
		bits++;  // Fall through.
	case 2:
		bits++;  // Fall through.
	case 1:
		break;
	}
	IOC_ASSERT((1 << bits) == original_x);
	return bits;
	return 0;
}

/// @brief 虽然大多数编译器的表现没有问题，但是C++标准未指定'>>'的语义，因此这里使用函数以方便移植。
static inline int ArithmeticShiftRight(int x, int s) {
	return x >> s;
}

/// @brief 计算类型T的变量x的0基偏移，这可以安全的把指针转化为intptr_t.
template <typename T>
static inline intptr_t OffsetFrom(T x) {
	return x - static_cast<T>(0);
}

/// @brief OffsetFrom函数的逆函数，将intptr_t转化为指针.
template <typename T>
static inline T AddressFrom(intptr_t x) {
	return static_cast<T>(static_cast<T>(0) + x);
}

/// @brief Return the largest multiple of m which is <= x.
template <typename T>
static inline T RoundDown(T x, int m) {
	IOC_ASSERT(IsPowerOf2(m));
	return AddressFrom<T>(OffsetFrom(x) & -m);
}
/// @brief Return the smallest multiple of m which is >= x.
template <typename T>
static inline T RoundUp(T x, int m) {
	return RoundDown(x + m - 1, m);
}

/// @brief compare a and b.相等返回0，a<b返回-1，否则返回1.
template <typename T>
static int Compare(const T& a, const T& b) {
	if (a == b)
		return 0;
	else if (a < b)
		return -1;
	else
		return 1;
}

template <typename T>
static int PointerValueCompare(const T* a, const T* b) {
	return Compare<T>(*a, *b);
}

/** @brief Returns the smallest power of two which is >= x.
 * @details If you pass in a number that is already a power of two, 
 * it is returned as is.Implementation is from "Hacker's Delight"
 * by Henry S. Warren, Jr.
 * figure 3-3, page 48, where the function is called clp2.
 */
static inline uint32_t RoundUpToPowerOf2(uint32_t x) {
	IOC_ASSERT(x <= 0x80000000u);
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}
template <typename T>
static inline bool IsAligned(T value, T alignment) {
	IOC_ASSERT(IsPowerOf2(alignment));
	return (value & (alignment - 1)) == 0;
}
/// @brief Returns true if (addr + offset) is aligned.
static inline bool IsAddressAligned(Address addr,
                                    intptr_t alignment,
                                    int offset) {
	intptr_t offs = OffsetFrom(addr + offset);
	return IsAligned(offs, alignment);
}
/// @brief Returns the maximum of the two parameters.
template <typename T>
static T Max(T a, T b) {
	return a < b ? b : a;
}
/// @brief Returns the minimum of the two parameters.
template <typename T>
static T Min(T a, T b) {
	return a < b ? a : b;

/** @} */ //end of basicMath

}

/** @defgroup string String Operation
 *  @ingroup utils
 *  @{
 **/

/// @brief Optimized string comparison.
#define STREQ(a, b) (*(a) == *(b) && strcmp((a), (b)) == 0)

/** @} */ //end of string

} // namespace ioc

/** @} */ //end of utils

#endif //IOC_UTILS_MACRO_H
