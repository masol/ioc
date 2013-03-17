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

#ifndef  IOC_UTILS_EXCEPTION_H
#define  IOC_UTILS_EXCEPTION_H

#include <iostream>
#include <exception>
#include <sstream>

namespace ioc
{

/**
 * @brief           ioc's exception
 * @usage           IOC_THROW(message)
 **/
class exception: public std::exception
{
    public:
        exception(std::size_t line,
                    const char * file,
                    const char * message)throw()
        {
            try{
                std::stringstream ss;
                ss<<"exception: ["<<line<<" in "<<file<<"]"<<message;
                m_message=ss.str();
            }catch(...)
            {}
        }

        virtual ~exception()throw() {}

        virtual const char * what() const throw()
        {
            return m_message.c_str();
        }

    private:
        std::string m_message;
};

#define IOC_THROW(message) throw ioc::exception(__LINE__, __FILE__, message)


}
#endif   /* ----- #ifndef IOC_UTILS_EXCEPTION_H  ----- */

