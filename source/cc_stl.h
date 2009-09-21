/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_stl.h
// 
//

#if !defined(__CC_STL_H__) || !defined(INCLUDE_GUARDS)
#define __CC_STL_H__

#include <string>
#include <vector>
#include <list>

template<typename T>
class GameAllocator
{
public : 
	//    typedefs

	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

public : 

	//    convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind
	{
		typedef GameAllocator<U> other;
	};

public : 
	inline explicit GameAllocator()
	{
	}
	inline ~GameAllocator()
	{
	}
	inline explicit GameAllocator(GameAllocator const&)
	{
	}
	template<typename U>
	inline explicit GameAllocator(GameAllocator<U> const&)
	{
	}

	//    address
	inline pointer address(reference r)
	{
		return &r;
	}
	inline const_pointer address(const_reference r)
	{
		return &r;
	}

	//    memory allocation
	inline pointer allocate(size_type cnt, 
		typename std::allocator<void>::const_pointer = 0)
	{ 
			return reinterpret_cast<pointer>(QNew (com_gamePool, 0) (cnt * sizeof (T))); 
	}
	inline void deallocate(pointer p, size_type)
	{ 
		QDelete (p); 
	}

	//    size
	inline size_type max_size() const
	{ 
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	//    construction/destruction
	inline void construct(pointer p, const T& t)
	{
		new(p) T(t);
	}
	inline void destroy(pointer p)
	{
		p->~T();
	}

	inline bool operator == (GameAllocator const&)
	{
		return true;
	}
	inline bool operator != (GameAllocator const& a)
	{
		return !(operator == (a));
	}
};

#else
FILE_WARNING
#endif