#pragma once
#ifdef EE_VULKAN

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		// re-export all vulkan resource handle
		//using VulkanSemaphore = VkSemaphore;
		using VulkanPipeline = VkPipeline;
	}
}

#endif