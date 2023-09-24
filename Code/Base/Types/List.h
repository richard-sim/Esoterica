#pragma once

#include <EASTL/list.h>
#include <EASTL/fixed_list.h>

namespace EE
{
    template <typename E> using TList = eastl::list<E>;

    template<typename T, eastl_size_t S> using TInlineList = eastl::fixed_list<T, S, true>;
    template<typename T, eastl_size_t S> using TFixedList = eastl::fixed_list<T, S, false>;
}