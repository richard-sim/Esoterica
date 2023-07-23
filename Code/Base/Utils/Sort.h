#pragma once

#include "Base/Types/Arrays.h"

#include <EASTL/sort.h>

namespace EE
{
	template <typename T, typename Pred>
	void VectorSort( TVector<T>& arrays, Pred&& pred )
	{
		eastl::sort( arrays.begin(), arrays.end(), pred );
	}
}