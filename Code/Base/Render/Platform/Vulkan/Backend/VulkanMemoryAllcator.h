#pragma once
#ifdef EE_VULKAN

#include <vma/vk_mem_alloc.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanMemoryAllocator
		{
		public:

			struct InitConfig
			{

			};

		public:

			VulkanMemoryAllocator( InitConfig const& config );

		private:

			VmaAllocator				m_pAllocator;
		};
	}
}

#endif