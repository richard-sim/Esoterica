#if defined(EE_VULKAN)
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "VulkanCommonSettings.h"
#include "RHIToVulkanSpecification.h"
#include "Base/Logging/Log.h"
#include "Base/Types/Arrays.h"

namespace EE::Render
{
    namespace Backend
    {
        bool VulkanFramebufferCache::Initialize( VulkanRenderPass* pRenderPass, RHI::RHIRenderPassCreateDesc const& createDesc )
        {
            if ( !m_bIsInitialized && pRenderPass != nullptr )
            {
                EE_ASSERT( m_pRenderPass == nullptr );
                m_pRenderPass = pRenderPass;

                EE_ASSERT( createDesc.IsValid() );
                for ( auto const& colorAttachment : createDesc.m_colorAttachments )
                {
                    m_attachmentDescs.push_back( colorAttachment );
                }

                m_colorAttachmentCount = static_cast<uint32_t>( m_attachmentDescs.size() );

                if ( createDesc.m_depthAttachment.has_value() )
                {
                    m_attachmentDescs.push_back( createDesc.m_depthAttachment.value() );
                }

                m_bIsInitialized = true;
                return true;
            }

            return false;
        }

        void VulkanFramebufferCache::ClearUp( VulkanDevice* pDevice )
        {
            if ( pDevice != nullptr && m_bIsInitialized )
            {
                for ( auto& framebuffer : m_cachedFrameBuffers )
                {
                    vkDestroyFramebuffer( pDevice->m_pHandle, framebuffer.second, nullptr );
                }

                m_cachedFrameBuffers.clear();
                m_attachmentDescs.clear();
                m_pRenderPass = nullptr;
                m_bIsInitialized = false;
            }
            else
            {
                EE_LOG_WARNING("Render", "VulkanFrameBufferCache::ClearUp", "Trying to call ClearUp() on uninitialized VulkanFrameBufferCache!");
            }
        }

        VkFramebuffer VulkanFramebufferCache::GetOrCreate( VulkanDevice* pDevice, VulkanFramebufferCacheKey const& key )
        {
            EE_ASSERT( m_bIsInitialized );
            EE_ASSERT( m_attachmentDescs.size() == key.m_attachmentHashs.size() );
            EE_ASSERT( key.m_extentX != 0 && key.m_extentY != 0 );

            auto iter = m_cachedFrameBuffers.find( key );
            if ( iter != m_cachedFrameBuffers.end() )
            {
                return iter->second;
            }

            TFixedVector<VkFramebufferAttachmentImageInfo, RHI::RHIRenderPassCreateDesc::NumMaxAttachmentCount> attachmentImageInfos;
            TFixedVector<VkFormat, RHI::RHIRenderPassCreateDesc::NumMaxAttachmentCount> attachmentImageFormats;

            for ( size_t i = 0; i < m_attachmentDescs.size(); ++i )
            {
                auto const& hash = key.m_attachmentHashs[i];
                attachmentImageFormats.push_back( ToVulkanFormat( m_attachmentDescs[i].m_pixelFormat ) );

                VkFramebufferAttachmentImageInfo attachmentImageInfo = {};
                attachmentImageInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
                attachmentImageInfo.width = key.m_extentX;
                attachmentImageInfo.height = key.m_extentY;
                attachmentImageInfo.usage = hash.m_imageUsageFlags;
                attachmentImageInfo.flags = hash.m_imageCreateFlags;
                attachmentImageInfo.layerCount = 1;
                attachmentImageInfo.viewFormatCount = 1;
                attachmentImageInfo.pViewFormats = &attachmentImageFormats[i];
                attachmentImageInfos.push_back( attachmentImageInfo );
            }

            VkFramebufferAttachmentsCreateInfo attachmentsCI = {};
            attachmentsCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO;
            attachmentsCI.attachmentImageInfoCount = static_cast<uint32_t>( attachmentImageInfos.size() );
            attachmentsCI.pAttachmentImageInfos = attachmentImageInfos.data();

            VkFramebufferCreateInfo framebufferCI = {};
            framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCI.pNext = &attachmentsCI;
            framebufferCI.width = key.m_extentX;
            framebufferCI.height = key.m_extentY;
            framebufferCI.renderPass = m_pRenderPass->m_pHandle;
            framebufferCI.layers = 1;
            // Note: lazy bound images
            framebufferCI.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
            framebufferCI.attachmentCount = static_cast<uint32_t>( attachmentImageInfos.size() );
            // Note: we specified VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT flag, so this can be nullptr
            framebufferCI.pAttachments = nullptr;

            VkFramebuffer newFramebuffer;
            VK_SUCCEEDED( vkCreateFramebuffer( pDevice->m_pHandle, &framebufferCI, nullptr, &newFramebuffer ) );

            m_cachedFrameBuffers.insert( { key, newFramebuffer } );

            return newFramebuffer;
        }
    }
}

#endif