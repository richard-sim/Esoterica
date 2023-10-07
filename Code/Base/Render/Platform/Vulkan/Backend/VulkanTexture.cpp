#if defined(EE_VULKAN)
#include "VulkanTexture.h"
#include "VulkanDevice.h"
#include "VulkanCommon.h"
#include "RHIToVulkanSpecification.h"
#include "Base/RHI/RHIDowncastHelper.h"

namespace EE::Render
{
    namespace Backend
    {
        RHI::RHITextureView* VulkanTexture::CreateView( RHI::RHIDevice* pDevice, RHI::RHITextureViewCreateDesc const& desc )
        {
            auto* pVkDevice = RHI::RHIDowncast<VulkanDevice>( pDevice );
            if ( !pVkDevice )
            {
                return nullptr;
            }

            uint32_t layerCount = 1;
            if ( m_desc.m_type == RHI::ETextureType::TCubemap || m_desc.m_type == RHI::ETextureType::TCubemapArray )
            {
                layerCount = 6;
            }

            VkImageViewCreateInfo viewCI = {};
            viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewCI.components.r = VK_COMPONENT_SWIZZLE_R;
            viewCI.components.g = VK_COMPONENT_SWIZZLE_G;
            viewCI.components.b = VK_COMPONENT_SWIZZLE_B;
            viewCI.components.a = VK_COMPONENT_SWIZZLE_A;
            viewCI.image = m_pHandle;
            viewCI.format = desc.m_format.has_value() ? ToVulkanFormat( *desc.m_format ) : ToVulkanFormat( m_desc.m_format );
            viewCI.viewType = desc.m_viewType.has_value() ? ToVulkanImageViewType( *desc.m_viewType ) : ToVulkanImageViewType( m_desc.m_type );
            viewCI.subresourceRange.baseMipLevel = desc.m_baseMipmap;
            viewCI.subresourceRange.baseArrayLayer = 0;
            viewCI.subresourceRange.aspectMask = ToVulkanImageAspectFlags( desc.m_viewAspect );
            viewCI.subresourceRange.levelCount = desc.m_levelCount.has_value() ? *desc.m_levelCount : m_desc.m_mipmap - desc.m_baseMipmap;
            viewCI.subresourceRange.layerCount = layerCount;

            auto* pVkTextureView = EE::New<VulkanTextureView>();
            if ( !pVkTextureView )
            {
                EE_ASSERT( false );
            }
            VK_SUCCEEDED( vkCreateImageView( pVkDevice->m_pHandle, &viewCI, nullptr, &(pVkTextureView->m_pHandle) ) );

            return pVkTextureView;
        }

        void VulkanTexture::DestroyView( RHI::RHIDevice* pDevice, RHI::RHITextureView* pTextureView )
        {
            auto* pVkDevice = RHI::RHIDowncast<VulkanDevice>( pDevice );
            if ( !pVkDevice )
            {
                EE_ASSERT( false );
                return;
            }

            auto* pVkTextureView = RHI::RHIDowncast<VulkanTextureView>( pTextureView );
            if ( !pVkTextureView )
            {
                EE_ASSERT( false );
                return;
            }

            vkDestroyImageView( pVkDevice->m_pHandle, pVkTextureView->m_pHandle, nullptr );

            EE::Delete( pTextureView );
        }
    }
}

#endif