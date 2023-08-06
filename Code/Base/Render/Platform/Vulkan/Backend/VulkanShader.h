#pragma once
#ifdef EE_VULKAN

#include "Base/RHI/Resource/RHIShader.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
    namespace Backend
    {
        class VulkanShader final : public RHI::RHIShader
        {
        public:

            ~VulkanShader() = default;

        private:

            friend class VulkanDevice;

            VkShaderModule              m_pModule = nullptr;
        };
    }
}

#endif // EE_VULKAN