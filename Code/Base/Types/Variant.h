#pragma once

#include <EASTL/variant.h>

namespace EE
{
	template <typename... Types> using TVariant = eastl::variant<Types ...>;
}