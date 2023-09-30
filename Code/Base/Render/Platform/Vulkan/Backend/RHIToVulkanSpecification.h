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

        // TODO: merge these two together.
        VkFormat ToVulkanFormat( RHI::EPixelFormat format );
        VkFormat ToVulkanFormat( Render::DataFormat format );

        VkImageType ToVulkanImageType( RHI::ETextureType type );
        VkSampleCountFlagBits ToVulkanSampleCountFlags( TBitFlags<RHI::ESampleCount> sample );
        VkImageUsageFlagBits ToVulkanImageUsageFlags( TBitFlags<RHI::ETextureUsage> usage );
        //VkImageLayout ToVulkanImageLayout();
        VkImageTiling ToVulkanImageTiling( RHI::ETextureMemoryTiling tiling );
        VkImageCreateFlagBits ToVulkanImageCreateFlags( TBitFlags<RHI::ETextureCreateFlag> createFlag );
        VkBufferUsageFlagBits ToVulkanBufferUsageFlags( TBitFlags<RHI::EBufferUsage> usage );

        //-------------------------------------------------------------------------

        VkAttachmentLoadOp ToVulkanAttachmentLoadOp( RHI::ERenderPassAttachmentLoadOp loadOP );
        VkAttachmentStoreOp ToVulkanAttachmentStoreOp( RHI::ERenderPassAttachmentStoreOp storeOP );
        VkAttachmentDescription ToVulkanAttachmentDescription( RHI::RHIRenderPassAttachmentDesc const& attachmentDesc );

        //-------------------------------------------------------------------------

        VkDescriptorType ToVulkanDescriptorType( Render::Shader::ReflectedBindingResourceType reflectedBindingType );

        //-------------------------------------------------------------------------
    
        VkShaderStageFlagBits ToVulkanShaderStageFlags( TBitFlags<Render::PipelineStage> pipelineStage );
        VkPrimitiveTopology ToVulkanPrimitiveTopology( RHI::ERHIPipelinePirmitiveTopology topology );
        VkCullModeFlagBits ToVulkanCullModeFlags( Render::CullMode cullMode );
        VkFrontFace ToVulkanFrontFace( Render::WindingMode windingMode );
        VkPolygonMode ToVulkanPolygonMode( Render::FillMode fillMode );
        VkBlendFactor ToVulkanBlendFactor( Render::BlendValue blendValue );
        VkBlendOp ToVulkanBlendOp( Render::BlendOp blendOp );
    }
}

#endif // EE_VULKAN