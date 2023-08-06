#pragma once
#ifdef EE_VULKAN

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanCommandBuffer
		{
            friend class VulkanDevice;

		public:

			VulkanCommandBuffer() = default;

			VulkanCommandBuffer( VulkanCommandBuffer const& ) = delete;
			VulkanCommandBuffer& operator=( VulkanCommandBuffer const& ) = delete;

			VulkanCommandBuffer( VulkanCommandBuffer&& ) = default;
			VulkanCommandBuffer& operator=( VulkanCommandBuffer&& ) = default;

		public:

			inline VkCommandBuffer Raw() const { return m_pHandle; }

		private:

			VkCommandBuffer					m_pHandle;
		};
	}
}

#endif