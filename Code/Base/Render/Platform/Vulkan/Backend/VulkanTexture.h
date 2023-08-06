#pragma once
#ifdef EE_VULKAN

#include "Base/Math/Math.h"
#include "Base/RHI/Resource/RHITexture.h"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanTexture : public RHI::RHITexture
		{
            friend class VulkanTexture;
			friend class VulkanSwapchain;

		public:

			VulkanTexture() = default;

		private:

			VkImage							m_pHandle = nullptr;
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