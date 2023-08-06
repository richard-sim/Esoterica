#pragma once
#ifdef EE_VULKAN

#include "Base/RHI/Resource/RHISemaphore.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanSemaphore final : public RHI::RHISemaphore
		{
			friend class VulkanDevice;

		public:

			VulkanSemaphore() = default;

		private:

			VkSemaphore						m_pHandle = nullptr;
		};
	}
}

#endif