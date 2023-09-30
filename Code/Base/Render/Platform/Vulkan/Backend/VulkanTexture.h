#pragma once
#if defined(EE_VULKAN)

#include "Base/Math/Math.h"
#include "Base/RHI/Resource/RHITexture.h"
#include "VulkanCommonSettings.h"

#include <optional>
#include <vulkan/vulkan_core.h>
#if VULKAN_USE_VMA_ALLOCATION
#include <vma/vk_mem_alloc.h>
#endif

namespace EE::Render
{
	namespace Backend
	{
		class VulkanTexture : public RHI::RHITexture
		{
            friend class VulkanDevice;
			friend class VulkanSwapchain;

		public:

            EE_RHI_STATIC_TAGGED_TYPE( RHI::ERHIType::Vulkan )

            VulkanTexture()
                : RHITexture( RHI::ERHIType::Vulkan )
            {}
            virtual ~VulkanTexture() = default;

		private:

			VkImage							m_pHandle = nullptr;
            #if VULKAN_USE_VMA_ALLOCATION
            VmaAllocation                   m_allocation = nullptr;
            #else
            VkDeviceMemory                  m_allocation = nullptr;
            #endif // VULKAN_USE_VMA_ALLOCATION
		};

		struct ImageViewDesc
		{
			std::optional<VkImageViewType>	m_type;
			std::optional<VkFormat>			m_format;
			VkImageAspectFlags				m_aspect;
			uint32_t						m_baseMipLevel;
			std::optional<uint32_t>			m_levelCount;
		};

		class VulkanImageView
		{
		public:

		private:

			VkImageView						m_pHandle = nullptr;
		};
	}
}

#endif