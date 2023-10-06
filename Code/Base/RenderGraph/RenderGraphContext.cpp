#include "RenderGraphContext.h"
#include "Base/RHI/RHIDevice.h"
#include "Base/RHI/Resource/RHITexture.h"
#include "Base/RHI/Resource/RHIRenderPass.h"
#include "Base/RHI/RHICommandBuffer.h"

namespace EE
{
    namespace RG
    {
        void RGRenderCommandContext::Reset()
        {
            m_pRenderGraph = nullptr;
            m_pDevice = nullptr;
            m_pCommandBuffer = nullptr;
        }

        //-------------------------------------------------------------------------

        bool RGRenderCommandContext::BeginRenderPass(
            RHI::RHIRenderPass* pRenderPass, Int2 extent,
            TSpan<RGRenderTargetViewDesc> colorAttachemnts,
            TOptional<RGRenderTargetViewDesc> depthAttachment
        )
        {
            EE_ASSERT( m_pCommandBuffer );
            EE_ASSERT( extent.m_x > 0 && extent.m_y > 0 );

            // get or create necessary framebuffer
            //-------------------------------------------------------------------------

            RHI::RHIFramebufferCacheKey key;
            key.m_extentX = static_cast<uint32_t>( extent.m_x );
            key.m_extentX = static_cast<uint32_t>( extent.m_y );

            for ( auto const& color : colorAttachemnts )
            {
                auto const& desc = color.m_attachement.GetDesc().m_desc;
                key.m_attachmentHashs.emplace_back( desc.m_usage, desc.m_flag );
            }

            if ( depthAttachment.has_value() )
            {
                auto const& desc = depthAttachment->m_attachement.GetDesc().m_desc;
                key.m_attachmentHashs.emplace_back( desc.m_usage, desc.m_flag );
            }

            auto* pFramebuffer = pRenderPass->GetOrCreateFramebuffer( m_pDevice, key );
            if ( !pFramebuffer )
            {
                EE_LOG_WARNING( "RenderGraph", "RenderGraphCommandContext", "Failed to fetch framebuffer!" );
                return false;
            }

            // collect texture views
            //-------------------------------------------------------------------------

            TFixedVector<RHI::RHITextureView*, RHI::RHIRenderPassCreateDesc::NumMaxAttachmentCount> attachmentViews;

            //for ( auto const& color : colorAttachemnts )
            //{
            //    color.m_attachement.
            //}

            if ( depthAttachment.has_value() )
            {
                auto const& desc = depthAttachment->m_attachement.GetDesc().m_desc;
                key.m_attachmentHashs.emplace_back( desc.m_usage, desc.m_flag );
            }

            auto renderArea = RHI::RenderArea{ key.m_extentX, key.m_extentY, 0u, 0u };
            return m_pCommandBuffer->BeginRenderPass( pRenderPass, renderArea );
        }
    }
}
