#ifdef EE_VULKAN
#include "VulkanTexture.h"

namespace EE::Render
{
	namespace Backend
	{
		TextureDesc TextureDesc::GetDefault()
		{
			TextureDesc desc = {};
			desc.m_width = 0;
			desc.m_height = 0;
			desc.m_depth = 0;

			desc.m_array = 1;
			desc.m_mipmap = 1;

			// we can infer usage by its resource barrier type, so user do not need to explicitly fill in here,
			// but we still give user choice to add usage flags if needed.
			desc.m_usage = 0;
			desc.m_tiling = VK_IMAGE_TILING_OPTIMAL;
			desc.m_format = VK_FORMAT_UNDEFINED;
			desc.m_flags = VkFlags( 0 );
			desc.m_sample = VK_SAMPLE_COUNT_1_BIT;
			desc.m_type = VK_IMAGE_TYPE_2D;
			return desc;
		}

		TextureDesc TextureDesc::New1D( uint32_t width, VkFormat format )
		{
			auto desc = TextureDesc::GetDefault();
			desc.m_width = width;
			desc.m_height = 1;
			desc.m_depth = 1;

			desc.m_format = format;
			desc.m_type = VK_IMAGE_TYPE_1D;
			return desc;
		}

		TextureDesc TextureDesc::New1DArray( uint32_t width, VkFormat format, uint32_t array )
		{
			auto desc = TextureDesc::GetDefault();
			desc.m_width = width;
			desc.m_height = 1;
			desc.m_depth = 1;

			desc.m_format = format;
			desc.m_type = VK_IMAGE_TYPE_1D;
			desc.m_array = array;
			return desc;
		}

		TextureDesc TextureDesc::New2D( uint32_t width, uint32_t height, VkFormat format )
		{
			auto desc = TextureDesc::GetDefault();
			desc.m_width = width;
			desc.m_height = height;
			desc.m_depth = 1;

			desc.m_format = format;
			desc.m_type = VK_IMAGE_TYPE_2D;
			return desc;
		}

		TextureDesc TextureDesc::New2DArray( uint32_t width, uint32_t height, VkFormat format, uint32_t array )
		{
			auto desc = TextureDesc::GetDefault();
			desc.m_width = width;
			desc.m_height = height;
			desc.m_depth = 1;

			desc.m_format = format;
			desc.m_type = VK_IMAGE_TYPE_2D;
			desc.m_array = array;
			return desc;
		}

		TextureDesc TextureDesc::New3D( uint32_t width, uint32_t height, uint32_t depth, VkFormat format )
		{
			auto desc = TextureDesc::GetDefault();
			desc.m_width = width;
			desc.m_height = height;
			desc.m_depth = depth;

			desc.m_format = format;
			desc.m_type = VK_IMAGE_TYPE_3D;
			return desc;
		}

		TextureDesc TextureDesc::NewCubemap( uint32_t width, VkFormat format )
		{
			auto desc = TextureDesc::GetDefault();
			desc.m_width = width;
			desc.m_height = width;
			desc.m_depth = 1;

			desc.m_format = format;
			desc.m_type = VK_IMAGE_TYPE_2D;
			desc.m_array = 6;
			desc.m_flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			return desc;
		}
	}
}

#endif