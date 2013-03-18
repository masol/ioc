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
 * @file        c_log.h
 * @brief      提供了log支持的c头文件，可以在C代码中使用。
 **/


#ifndef  IOC_UTILS_C_LOG_H
#define  IOC_UTILS_C_LOG_H

#define IOC_LOG_SEV_NORM 		0
#define IOC_LOG_SEV_DEBUG 		1
#define IOC_LOG_SEV_INFO 		2
#define IOC_LOG_SEV_NOTICE		3
#define IOC_LOG_SEV_WARN		4
#define IOC_LOG_SEV_ERROR		5
#define IOC_LOG_SEV_EMERG		6

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 记录一条日志。
 * @details 内部调用 来实现日志记录，
 * 因此本函数仅仅是C语言中使用日志的一个adapter，
 * 通常配合sprintf使用.
 */
void ioc_log(unsigned int severity,const char* msg);


/** @brief 当前日志系统是否启用。
 * @details 内部调用 来检查制定级别的日志是否被允许，
 * 本函数仅仅是C语言中使用日志的一个adapter.
 * @return 1 true,0 false.
 * @todo not implement.return 1 always.
 */
int ioc_log_isEanbled(unsigned int severity);


#ifdef __cplusplus
}
#endif

#endif //IOC_UTILS_C_LOG_H