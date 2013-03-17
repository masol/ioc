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

#ifndef IOC_UTILS_SINGLETON_H
#define IOC_UTILS_SINGLETON_H

#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

namespace ioc
{
    template<class T>
    class Singleton : private boost::noncopyable
    {
    public:
        static T& instance()
        {
            boost::call_once(T::init, m_once_flag);
            return *m_instance_ptr;
        }
        static void init()
        {
            m_instance_ptr.reset(new T());
        }
    protected:
        ~Singleton() {}
        Singleton() {}
    private:
        static boost::scoped_ptr<T> m_instance_ptr;
        static boost::once_flag     m_once_flag;
    };
}

template<class T> boost::scoped_ptr<T> ioc::Singleton<T>::m_instance_ptr(0);
template<class T> boost::once_flag ioc::Singleton<T>::m_once_flag = BOOST_ONCE_INIT;

#endif //IOC_UTILS_SINGLETON_H
