#pragma once
#ifdef EE_VULKAN

#include "Base/Math/Math.h"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		struct TextureDesc
		{
			static TextureDesc GetDefault();
			static TextureDesc New1D( uint32_t width, VkFormat format );
			static TextureDesc New1DArray( uint32_t width, VkFormat format, uint32_t array );
			static TextureDesc New2D( uint32_t width, uint32_t height, VkFormat format );
			static TextureDesc New2DArray( uint32_t width, uint32_t height, VkFormat format, uint32_t array );
			static TextureDesc New3D( uint32_t width, uint32_t height, uint32_t depth, VkFormat format );
			static TextureDesc NewCubemap( uint32_t width, VkFormat format );

			uint32_t						m_width;
			uint32_t						m_height;
			uint32_t						m_depth;

			uint32_t						m_array;
			uint16_t						m_mipmap;

			VkImageUsageFlags				m_usage;
			VkImageTiling					m_tiling;
			VkFormat						m_format;
			VkImageCreateFlags				m_flags;
			VkSampleCountFlags				m_sample;
			VkImageType						m_type;
		};

		class VulkanTexture
		{

			friend class VulkanSwapchain;

		public:

			VulkanTexture() = default;

			VulkanTexture( VulkanTexture const& ) = delete;
			VulkanTexture& operator=( VulkanTexture const& ) = delete;

			VulkanTexture( VulkanTexture&& ) = default;
			VulkanTexture& operator=( VulkanTexture&& ) = default;

		private:

			VkImage							m_pHandle = nullptr;
			TextureDesc						m_desc;
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