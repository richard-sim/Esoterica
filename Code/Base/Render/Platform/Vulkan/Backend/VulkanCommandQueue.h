#pragma once
#if defined(EE_VULKAN)

#include "Base/RHI/RHICommandQueue.h"
#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
    namespace Backend
    {
        class VulkanCommandQueue : public RHI::RHICommandQueue
        {
        public:

            enum class Type : uint8_t
            {
                Graphic,
                Compute,
                Transfer,
                Unknown
            };

        public:

            EE_RHI_STATIC_TAGGED_TYPE( RHI::ERHIType::Vulkan )

            VulkanCommandQueue()
                : RHI::RHICommandQueue( RHI::ERHIType::Vulkan )
            {}
            VulkanCommandQueue( VulkanDevice const& device, QueueFamily const& queueFamily );
            virtual ~VulkanCommandQueue() = default;

        public:

            inline bool IsValid() const { return m_pHandle != nullptr; }

            uint32_t GetQueueFamilyIndex() const { return m_queueFamily.m_index; }

        private:

            VkQueue								m_pHandle = nullptr;
            Type								m_type = Type::Unknown;
            QueueFamily							m_queueFamily;
        };
    }
}

#endif