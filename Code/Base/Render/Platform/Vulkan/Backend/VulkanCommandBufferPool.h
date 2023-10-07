#pragma once
#if defined(EE_VULKAN)

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
    namespace Backend
    {
        class VulkanCommandBufferPool
        {
            friend class VulkanDevice;

        public:

            VulkanCommandBufferPool() = default;

            VulkanCommandBufferPool( VulkanCommandBufferPool const& ) = delete;
            VulkanCommandBufferPool& operator=( VulkanCommandBufferPool const& ) = delete;

            VulkanCommandBufferPool( VulkanCommandBufferPool&& ) = delete;
            VulkanCommandBufferPool& operator=( VulkanCommandBufferPool&& ) = delete;

            //-------------------------------------------------------------------------

        private:

            VkCommandPool             m_pHandle = nullptr;
        };
    }
}

#endif