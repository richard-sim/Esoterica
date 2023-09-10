#pragma once
#if defined(EE_VULKAN)

#include "Base/Types/HashMap.h"
#include "Base/RHI/Resource/RHIPipelineState.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
    namespace Backend
    {
        class VulkanPipelineState : public RHI::RHIRasterPipelineState
        {
            friend class VulkanDevice;

            using SetDescriptorLayout = THashMap<uint32_t, VkDescriptorType>;

        public:

            VulkanPipelineState() = default;
            virtual ~VulkanPipelineState() = default;

        private:

            VkPipeline                                                          m_pPipeline;
            VkPipelineLayout                                                    m_pPipelineLayout;

            VkPipelineBindPoint                                                 m_pipelineBindPoint;

            TInlineVector<SetDescriptorLayout, RHI::MAX_DESCRIPTOR_SET_COUNT>   m_setDescriptorLayouts;
            TInlineVector<VkDescriptorPoolSize, RHI::MAX_DESCRIPTOR_SET_COUNT>  m_setPoolSizes;
            TInlineVector<VkDescriptorSetLayout, RHI::MAX_DESCRIPTOR_SET_COUNT> m_setLayouts;
        };
    }
}

#endif