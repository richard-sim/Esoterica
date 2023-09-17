#include "RHIToVulkanSpecification.h"
#if defined(EE_VULKAN)

namespace EE::Render
{
    namespace Backend
    {
        #if VULKAN_USE_VMA_ALLOCATION
        VmaMemoryUsage ToVmaMemoryUsage( RHI::ERenderResourceMemoryUsage memoryUsage )
        {
            switch ( memoryUsage )
            {
                case RHI::ERenderResourceMemoryUsage::CPUToGPU: return VMA_MEMORY_USAGE_CPU_TO_GPU;
                case RHI::ERenderResourceMemoryUsage::GPUToCPU: return VMA_MEMORY_USAGE_GPU_TO_CPU;
                case RHI::ERenderResourceMemoryUsage::CPUOnly: return VMA_MEMORY_USAGE_CPU_ONLY;
                case RHI::ERenderResourceMemoryUsage::GPUOnly: return VMA_MEMORY_USAGE_GPU_ONLY;
                case RHI::ERenderResourceMemoryUsage::CPUCopy: return VMA_MEMORY_USAGE_CPU_COPY;
                // TODO: transient attachment
                case RHI::ERenderResourceMemoryUsage::GPULazily: return VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
                default:
                break;
            }
            EE_UNREACHABLE_CODE();
            return VMA_MEMORY_USAGE_MAX_ENUM;
        }
        #endif // VULKAN_USE_VMA_ALLOCATION

        //-------------------------------------------------------------------------

		VkFormat ToVulkanFormat( RHI::EPixelFormat format )
		{
            switch ( format )
            {
                case RHI::EPixelFormat::RGBA8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
                case RHI::EPixelFormat::BGRA8Unorm: return VK_FORMAT_B8G8R8A8_UNORM;
                default:
                break;
            }
            EE_UNREACHABLE_CODE();
            return VK_FORMAT_UNDEFINED;
		}

        VkImageType ToVulkanImageType( RHI::ETextureType type )
        {
            switch ( type )
            {
                case RHI::ETextureType::T1D: return VK_IMAGE_TYPE_1D;
                case RHI::ETextureType::T1DArray: return VK_IMAGE_TYPE_1D;
                case RHI::ETextureType::T2D: return VK_IMAGE_TYPE_2D;
                case RHI::ETextureType::T2DArray: return VK_IMAGE_TYPE_2D;
                case RHI::ETextureType::T3D: return VK_IMAGE_TYPE_3D;
                case RHI::ETextureType::TCubemap: return VK_IMAGE_TYPE_2D;
                default:
                break;
            }
            EE_UNREACHABLE_CODE();
            return VK_IMAGE_TYPE_MAX_ENUM;
        }

        VkSampleCountFlagBits ToVulkanSampleCountFlags( TBitFlags<RHI::ETextureSampleCount> sample )
        {
            VkFlags flag = 0u;
            if ( sample.IsFlagSet( RHI::ETextureSampleCount::SC1 ) )
            {
                flag |= VK_SAMPLE_COUNT_1_BIT;
            }
            if ( sample.IsFlagSet( RHI::ETextureSampleCount::SC2 ) )
            {
                flag |= VK_SAMPLE_COUNT_2_BIT;
            }
            if ( sample.IsFlagSet( RHI::ETextureSampleCount::SC4 ) )
            {
                flag |= VK_SAMPLE_COUNT_4_BIT;
            }
            if ( sample.IsFlagSet( RHI::ETextureSampleCount::SC8 ) )
            {
                flag |= VK_SAMPLE_COUNT_8_BIT;
            }
            if ( sample.IsFlagSet( RHI::ETextureSampleCount::SC16 ) )
            {
                flag |= VK_SAMPLE_COUNT_16_BIT;
            }
            if ( sample.IsFlagSet( RHI::ETextureSampleCount::SC32 ) )
            {
                flag |= VK_SAMPLE_COUNT_32_BIT;
            }
            if ( sample.IsFlagSet( RHI::ETextureSampleCount::SC64 ) )
            {
                flag |= VK_SAMPLE_COUNT_64_BIT;
            }
            return VkSampleCountFlagBits( flag );
        }

        VkImageUsageFlagBits ToVulkanImageUsageFlags( TBitFlags<RHI::ETextureUsage> usage )
        {
            VkFlags flag = 0u;
            if ( usage.IsFlagSet( RHI::ETextureUsage::TransferSrc ) )
            {
                flag |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            }
            if ( usage.IsFlagSet( RHI::ETextureUsage::TransferDst ) )
            {
                flag |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            }
            if ( usage.IsFlagSet( RHI::ETextureUsage::Sampled ) )
            {
                flag |= VK_IMAGE_USAGE_SAMPLED_BIT;
            }
            if ( usage.IsFlagSet( RHI::ETextureUsage::Storage ) )
            {
                flag |= VK_IMAGE_USAGE_STORAGE_BIT;
            }
            if ( usage.IsFlagSet( RHI::ETextureUsage::Color ) )
            {
                flag |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
            if ( usage.IsFlagSet( RHI::ETextureUsage::DepthStencil ) )
            {
                flag |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
            if ( usage.IsFlagSet( RHI::ETextureUsage::Transient ) )
            {
                flag |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
            }
            if ( usage.IsFlagSet( RHI::ETextureUsage::Input ) )
            {
                flag |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            }
            return VkImageUsageFlagBits( flag );
        }

		VkImageTiling ToVulkanImageTiling( RHI::ETextureMemoryTiling tiling )
		{
            switch ( tiling )
            {
                case RHI::ETextureMemoryTiling::Optimal: return VK_IMAGE_TILING_OPTIMAL;
                case RHI::ETextureMemoryTiling::Linear: return VK_IMAGE_TILING_LINEAR;
                default:
                break;
            }
            EE_UNREACHABLE_CODE();
            return VK_IMAGE_TILING_MAX_ENUM;
		}

        VkImageCreateFlagBits ToVulkanImageCreateFlags( TBitFlags<RHI::ETextureCreateFlag> createFlag )
        {
            VkFlags flag = 0u;
            if ( createFlag.IsFlagSet( RHI::ETextureCreateFlag::CubeCompatible ) )
            {
                flag |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            }
            return VkImageCreateFlagBits( flag );
        }

		VkBufferUsageFlagBits ToVulkanBufferUsageFlags( TBitFlags<RHI::EBufferUsage> usage )
		{
            VkFlags flag = 0u;
            if ( usage.IsFlagSet( RHI::EBufferUsage::TransferSrc ) )
            {
                flag |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::TransferDst ) )
            {
                flag |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::UniformTexel ) )
            {
                flag |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::StorageTexel ) )
            {
                flag |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::Uniform ) )
            {
                flag |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::Storage ) )
            {
                flag |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::Index ) )
            {
                flag |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::Vertex ) )
            {
                flag |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::Indirect ) )
            {
                flag |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            }
            if ( usage.IsFlagSet( RHI::EBufferUsage::ShaderDeviceAddress ) )
            {
                flag |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
            }
            return VkBufferUsageFlagBits( flag );
		}

        //-------------------------------------------------------------------------

		VkDescriptorType ToVulkanDescriptorType( Render::Shader::ReflectedBindingResourceType reflectedBindingType )
		{
            // TODO: support dynamic uniform buffer and storage buffer
            switch ( reflectedBindingType )
            {
                case EE::Render::Shader::ReflectedBindingResourceType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
                case EE::Render::Shader::ReflectedBindingResourceType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                case EE::Render::Shader::ReflectedBindingResourceType::SampledImage: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                case EE::Render::Shader::ReflectedBindingResourceType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                case EE::Render::Shader::ReflectedBindingResourceType::UniformTexelBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                case EE::Render::Shader::ReflectedBindingResourceType::StorageTexelBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
                case EE::Render::Shader::ReflectedBindingResourceType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                case EE::Render::Shader::ReflectedBindingResourceType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                case EE::Render::Shader::ReflectedBindingResourceType::InputAttachment: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                default:
                EE_UNREACHABLE_CODE();
                break;
            }
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}
}

#endif