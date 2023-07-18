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

			VulkanPipeline() = default;

			VulkanPipeline( VulkanPipeline const& ) = delete;
			VulkanPipeline& operator=( VulkanPipeline const& ) = delete;

			VulkanPipeline( VulkanPipeline&& ) = default;
			VulkanPipeline& operator=( VulkanPipeline&& ) = default;

		public:



		private:

			VkPipeline					m_pHandle;
		};
	}
}

#endif