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

#include "config.h"
#include "utils/pathext.h"

#if (BOOST_FILESYSTEM_VERSION != 3)
#ifndef WIN32
#include <unistd.h> 
#endif //WIN32
#endif //(BOOST_FILESYSTEM_VERSION != 3)

namespace boost{
namespace filesystem{
namespace ioc_ext{
	
	bool read_symlink(boost::filesystem::path &result)
	{
#if (BOOST_FILESYSTEM_VERSION == 3)
		result = boost::filesystem::read_symlink(result);
		return true;
#elif !defined(WIN32)
		char	buf[512];
		if(readlink(result.string().c_str(),buf,512) > 0)
		{
			result = boost::filesystem::path(buf);
			return true;
		}
		return false;
#else
		return false;
#endif //WIN32		
	}

	boost::filesystem::path canonicalize(const boost::filesystem::path& p)
	{
		boost::filesystem::path result;
		for(boost::filesystem::path::iterator it=p.begin();
			it!=p.end();
			++it)
		{
			if(*it == "..")
			{
				// /a/b/.. is not necessarily /a if b is a symbolic link
				if(boost::filesystem::is_symlink(result) )
					result /= *it;
				// /a/b/../.. is not /a/b/.. under most circumstances
				// We can end up with ..s in our result because of symbolic links
				else if(result.filename() == "..")
					result /= *it;
				// Otherwise it should be safe to resolve the parent
				else
					result = result.parent_path();
			}
			else if(*it == ".")
			{
				// Ignore
			}
			else
			{
				// Just cat other path entries
				result /= *it;
			}
		}
		return result;
	}
	
	
	boost::filesystem::path resolve_sym(const boost::filesystem::path& p)
	{
		boost::filesystem::path result;
		for(boost::filesystem::path::iterator it=p.begin();
			it!=p.end();
			++it)
		{
			if(*it == "..")
			{
				// /a/b/.. is not necessarily /a if b is a symbolic link
				if(boost::filesystem::is_symlink(result) )
				{
					read_symlink(result);
					result /= *it;
				}
				// /a/b/../.. is not /a/b/.. under most circumstances
				// We can end up with ..s in our result because of symbolic links
				else if(result.filename() == "..")
					result /= *it;
				// Otherwise it should be safe to resolve the parent
				else
					result = result.parent_path();
			}
			else if(*it == ".")
			{
				// Ignore
			}
			else
			{
				// Just cat other path entries
				result /= *it;
			}
			if(boost::filesystem::is_symlink(result) )
			{
				read_symlink(result);
			}
		}
		return result;
	}
	
	
}//ioc_ext
}//boost
}//filesystem

	