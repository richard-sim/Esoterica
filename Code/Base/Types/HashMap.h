#pragma once

#include "Base/Memory/Memory.h"
#include <EASTL/hash_map.h>
#include <EASTL/fixed_hash_map.h>

//-------------------------------------------------------------------------

namespace EE
{
    template<typename K, typename V> using THashMap = eastl::hash_map<K, V>;

    template<typename K, typename V, size_t NodeCount> using TInlineHashMap = eastl::fixed_hash_map<K, V, NodeCount>;
    template<typename K, typename V, size_t NodeCount> using TFixedHashMap = eastl::fixed_hash_map<K, V, NodeCount>;
}