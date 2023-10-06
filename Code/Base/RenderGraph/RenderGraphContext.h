#pragma once

#include "RenderGraphNode.h"
#include "Base/Math/Math.h"
#include "Base/Types/Arrays.h"
#include "Base/Types/Optional.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"

namespace EE::RHI
{
    class RHIDevice;
    class RHICommandBuffer;
    class RHIRenderPass;
}

namespace EE
{
	namespace RG
	{
		class RenderGraphContext
		{
		public:

		private:


		};

        //-------------------------------------------------------------------------

        struct RGRenderTargetViewDesc
        {
            RGNodeResourceRef<RGResourceTagTexture, RGResourceViewType::RT>     m_attachement;
            RHI::RHITextureViewCreateDesc                                       m_viewDesc;
        };

        class RGRenderCommandContext
        {
            friend class RenderGraph;

        public:

            //-------------------------------------------------------------------------

            inline RHI::RHICommandBuffer* const& GetRHICommandBuffer() const { return m_pCommandBuffer; }

            bool BeginRenderPass(
                RHI::RHIRenderPass* pRenderPass, Int2 extent,
                TSpan<RGRenderTargetViewDesc> colorAttachemnts,
                TOptional<RGRenderTargetViewDesc> depthAttachment
            );
            inline void EndRenderPass() { EE_ASSERT( m_pCommandBuffer ); m_pCommandBuffer->EndRenderPass(); }

        private:

            // functions only accessible by RenderGraph.
            inline void SetCommandContext( RenderGraph const* pRenderGraph, RHI::RHIDevice* pDevice, RHI::RHICommandBuffer* pCommandBuffer )
            {
                m_pRenderGraph = pRenderGraph;
                m_pDevice = pDevice;
                m_pCommandBuffer = pCommandBuffer;
            }
            inline bool IsValid() const { return m_pRenderGraph && m_pDevice && m_pCommandBuffer; }
            void Reset();

        private:

            RenderGraph const*                  m_pRenderGraph = nullptr;

            RHI::RHIDevice*                     m_pDevice = nullptr;
            RHI::RHICommandBuffer*              m_pCommandBuffer = nullptr;
        };
	}
}
