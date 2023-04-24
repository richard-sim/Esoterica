#pragma once
#ifdef EE_VULKAN

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanPipeline
		{
		public:

		private:

			VkPipeline					m_pHandle;
		};
	}
}

#endif