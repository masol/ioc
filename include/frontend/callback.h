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

#ifndef  IOC_FRONTEND_CALLBACK_H
#define  IOC_FRONTEND_CALLBACK_H

#include "utils/zone.h"
#include<boost/unordered_map.hpp>

namespace ioc{

class NameSapce;
class Call;

class Callback : public ioc::ZoneObject
{
private:
	/// @brief 某个回调指向的目标入口区块。
	NameSapce		*m_entryBlock;
	/// @brief 产生本回调的Call节点。如果为空意味着主线程。(文件入口)。
	Call			*m_forkPosition;
};


}



#endif //IOC_FRONTEND_CALLBACK_H