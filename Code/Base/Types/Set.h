#pragma once

#include <EASTL/unordered_set.h>
#include <EASTL/set.h>

namespace EE
{
	template <typename T> using TSet = eastl::set<T>;
	template <typename T> using TMultiSet = eastl::multiset<T>;
	template <typename T> using TUnorderedSet = eastl::unordered_set<T, eastl::hash<T>, eastl::equal_to<T>, eastl::allocator, false>;
	template <typename T> using TUnorderedMultiSet = eastl::unordered_multiset<T, eastl::hash<T>, eastl::equal_to<T>, eastl::allocator, false>;
}