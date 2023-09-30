#pragma once
#if defined(EE_VULKAN)

#include "VulkanFramebuffer.h"
#include "Base/Types/HashMap.h"
#include "Base/Types/Arrays.h"
#include "Base/RHI/Resource/RHIRenderPass.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
    namespace Backend
    {
        class VulkanRenderPass;
        class VulkanDevice;

        class VulkanFramebufferCache
        {
            using RHIRenderPassAttachmentDescs = TFixedVector<RHI::RHIRenderPassAttachmentDesc, RHI::RHIRenderPassCreateDesc::NumMaxAttachmentCount>;
        
        public:

            bool Initialize( VulkanRenderPass* pRenderPass, RHI::RHIRenderPassCreateDesc const& createDesc );
            void ClearUp( VulkanDevice* pDevice );

            VkFramebuffer GetOrCreate( VulkanDevice* pDevice, VulkanFramebufferCacheKey const& key );

            inline uint32_t GetColorAttachmentCount() const { return m_colorAttachmentCount; }

        private:

            THashMap<VulkanFramebufferCacheKey, VkFramebuffer>  m_cachedFrameBuffers;
            RHIRenderPassAttachmentDescs                        m_attachmentDescs;
            VulkanRenderPass*                                   m_pRenderPass = nullptr;
            uint32_t                                            m_colorAttachmentCount = 0;

            bool                                                m_bIsInitialized = false;
        };

        class VulkanRenderPass : public RHI::RHIRenderPass
        {
            friend class VulkanDevice;
            friend class VulkanFramebufferCache;

        public:

            EE_RHI_STATIC_TAGGED_TYPE( RHI::ERHIType::Vulkan )

            VulkanRenderPass()
                : RHIRenderPass( RHI::ERHIType::Vulkan )
            {}
            virtual ~VulkanRenderPass() = default;

        public:

            VulkanFramebufferCache          m_frameBufferCache;
        
        private:

            VkRenderPass				    m_pHandle = nullptr;
        };
    }
}

#endif