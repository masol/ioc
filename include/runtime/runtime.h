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

#ifndef  IOC_RUNTIME_RUNTIME_H
#define  IOC_RUNTIME_RUNTIME_H

/**
 * @file     app.h
 * @brief    维护应用程序信息。应用程序由xml框架以及内置的js代码构成。
 * @details  每段js被编译为一个函数。这个函数与xml文件产生的代码合并，以创建新的派生类型，派生类型的方法属性由js决定，js方法被链接到派生类的方法上。
 **/

#include "utils/singleton.h"

//前置声明
namespace llvm{
    class Module;
}

namespace ioc{

class Runtime : public Singleton<Runtime>
{
private:
    friend class Singleton<Runtime>;
    typedef Singleton<Runtime>   inherited;
    Runtime();
    llvm::Module*    m_appModule;
    std::string     m_pathName;
public:
    void pathName(const std::string &n);
    inline const std::string& pathName()const{
        return m_pathName;
    }
    ~Runtime();
    llvm::Module*   getModule();
};

} //end namespace ioc.

#endif	//IOC_RUNTIME_APP_H
