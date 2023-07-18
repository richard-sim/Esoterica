#pragma once
#ifdef EE_VULKAN

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanSemaphore
		{
		public:

			VulkanSemaphore() = default;

			VulkanSemaphore( VulkanSemaphore const& ) = delete;
			VulkanSemaphore& operator=( VulkanSemaphore const& ) = delete;

			VulkanSemaphore( VulkanSemaphore&& ) = default;
			VulkanSemaphore& operator=( VulkanSemaphore&& ) = default;

		private:

			friend class VulkanDevice;

			VkSemaphore						m_pHandle = nullptr;
		};
	}
}

#endif