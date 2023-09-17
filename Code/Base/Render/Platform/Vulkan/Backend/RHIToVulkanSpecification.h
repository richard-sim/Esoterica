#pragma once
#if defined(EE_VULKAN)
#include "VulkanCommonSettings.h"
#include "Base\RHI\Resource\RHIResourceCreationCommons.h"

#include <vulkan/vulkan_core.h>
#if VULKAN_USE_VMA_ALLOCATION
#include <vma/vk_mem_alloc.h>
#endif // VULKAN_USE_VMA_ALLOCATION

namespace EE::Render
{
    namespace Backend
    {
        // Utility function to convert RHI resource description to vulkan specific
        #if VULKAN_USE_VMA_ALLOCATION
        VmaMemoryUsage ToVmaMemoryUsage( RHI::ERenderResourceMemoryUsage memoryUsage );
        #else
        #error No implementation for default memory allocation usage convertion yet!
        #endif // VULKAN_USE_VMA_ALLOCATION

        VkFormat ToVulkanFormat( RHI::EPixelFormat format );
        VkImageType ToVulkanImageType( RHI::ETextureType type );
        VkSampleCountFlagBits ToVulkanSampleCountFlags( TBitFlags<RHI::ETextureSampleCount> sample );
        VkImageUsageFlagBits ToVulkanImageUsageFlags( TBitFlags<RHI::ETextureUsage> usage );
        //VkImageLayout ToVulkanImageLayout();
        VkImageTiling ToVulkanImageTiling( RHI::ETextureMemoryTiling tiling );
        VkImageCreateFlagBits ToVulkanImageCreateFlags( TBitFlags<RHI::ETextureCreateFlag> createFlag );
        VkBufferUsageFlagBits ToVulkanBufferUsageFlags( TBitFlags<RHI::EBufferUsage> usage );

        //-------------------------------------------------------------------------

        VkDescriptorType ToVulkanDescriptorType( Render::Shader::ReflectedBindingResourceType reflectedBindingType );
    }
}

#endif // EE_VULKAN