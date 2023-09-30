#pragma once
#if defined(EE_VULKAN)

#include "Base/Types/Map.h"
#include "Base/Render/RenderShader.h"
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

            using SetDescriptorLayout = TMap<uint32_t, VkDescriptorType>;

        public:

            EE_RHI_STATIC_TAGGED_TYPE( RHI::ERHIType::Vulkan )

            VulkanPipelineState()
                : RHIRasterPipelineState( RHI::ERHIType::Vulkan )
            {}
            virtual ~VulkanPipelineState() = default;

        private:

            VkPipeline                                                              m_pPipeline = nullptr;
            VkPipelineLayout                                                        m_pPipelineLayout = nullptr;

            VkPipelineBindPoint                                                     m_pipelineBindPoint;

            TInlineVector<SetDescriptorLayout, Shader::NumMaxResourceBindingSet>    m_setDescriptorLayouts;
            TInlineVector<VkDescriptorPoolSize, Shader::NumMaxResourceBindingSet>   m_setPoolSizes;
            TInlineVector<VkDescriptorSetLayout, Shader::NumMaxResourceBindingSet>  m_setLayouts;
        };
    }
}

#endif