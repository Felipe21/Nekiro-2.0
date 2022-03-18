/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2019  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_LOCKFREE_H
#define FS_LOCKFREE_H

#if _MSC_FULL_VER >= 190023918 // Workaround for VS2015 Update 2. Boost.Lockfree is a header-only library, so this should be safe to do.
#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#endif

/*
 * we use this to avoid instantiating multiple free lists for objects of the
 * same size and it can be replaced by a variable template in C++14
 *
 * template <size_t TSize, size_t Capacity>
 * boost::lockfree::stack<void*, boost::lockfree::capacity<Capacity> lockfreeFreeList;
 */
template <size_t TSize, size_t Capacity>
struct LockfreeFreeList
{
	using FreeList = boost::lockfree::stack<void*, boost::lockfree::capacity<Capacity>>;
	static FreeList& get()
	{
		static FreeList freeList;
		return freeList;
	}
};

template <typename T, size_t Capacity>
class LockfreePoolingAllocator
{
	public:
		template <class U>
		struct rebind
		{
			using other = LockfreePoolingAllocator<U, Capacity>;
		};

		LockfreePoolingAllocator() = default;

		template <typename U>
		explicit constexpr LockfreePoolingAllocator(const LockfreePoolingAllocator<U, Capacity>&) {}
		using value_type = T;

		T* allocate(size_t) const {
			auto& inst = LockfreeFreeList<sizeof(T), Capacity>::get();
			void* p; // NOTE: p doesn't have to be initialized
			if (!inst.pop(p)) {
				//Acquire memory without calling the constructor of T
				p = operator new (sizeof(T));
			}
			return static_cast<T*>(p);
		}

		void deallocate(T* p, size_t) const {
			auto& inst = LockfreeFreeList<sizeof(T), Capacity>::get();
			if (!inst.bounded_push(p)) {
				//Release memory without calling the destructor of T
				//(it has already been called at this point)
				operator delete(p);
			}
		}
};

#endif // FS_LOCKFREE_H
