#pragma once

#include <EASTL/atomic.h>

namespace EE
{
	typedef eastl::atomic<uint8_t>  AtomicU8;
	typedef eastl::atomic<uint16_t> AtomicU16;
	typedef eastl::atomic<uint32_t> AtomicU32;
	typedef eastl::atomic<uint64_t> AtomicU64;

	typedef eastl::atomic<int8_t>  AtomicI8;
	typedef eastl::atomic<int16_t> AtomicI16;
	typedef eastl::atomic<int32_t> AtomicI32;
	typedef eastl::atomic<int64_t> AtomicI64;

	template <typename T> using Atomic = eastl::atomic<T>;
}