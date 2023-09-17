#pragma once

#include <EASTL/map.h>
#include <EASTL/fixed_map.h>

namespace EE
{
    template <class K, class V> using TMap = eastl::map<K, V>;

    template <class K, class V, size_t NodeCount> using TInlineMap = eastl::fixed_map<K, V, NodeCount, true>;
    template <class K, class V, size_t NodeCount> using TFixedMap = eastl::fixed_map<K, V, NodeCount, false>;
}