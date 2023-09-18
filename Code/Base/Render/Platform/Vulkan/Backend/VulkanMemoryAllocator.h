#pragma once
#if defined(EE_VULKAN)

#include "Base/Memory/Pointers.h"
#include "VulkanCommonSettings.h"

#if VULKAN_USE_VMA_ALLOCATION
#define VK_API_VERSION_MAJOR(version) (((uint32_t)(version) >> 22) & 0x7FU)
#define VK_API_VERSION_MINOR(version) (((uint32_t)(version) >> 12) & 0x3FFU)
#define VK_API_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)
#include <vma/vk_mem_alloc.h>
#endif

namespace EE::Render
{
	namespace Backend
	{
        class VulkanDevice;

		class VulkanMemoryAllocator
		{
            friend class VulkanDevice;

		public:

			struct InitConfig
			{

			};

		public:

            VulkanMemoryAllocator() = default;
            ~VulkanMemoryAllocator();

            VulkanMemoryAllocator( VulkanMemoryAllocator const& ) = delete;
            VulkanMemoryAllocator& operator=( VulkanMemoryAllocator const& ) = delete;

            VulkanMemoryAllocator( VulkanMemoryAllocator&& ) = delete;
            VulkanMemoryAllocator& operator=( VulkanMemoryAllocator&& ) = delete;

            //-------------------------------------------------------------------------

            void Initialize( VulkanDevice* pDevice );
            void Shutdown();

		private:

            VulkanDevice*               m_pDevice = nullptr;

            #if VULKAN_USE_VMA_ALLOCATION
			VmaAllocator				m_pHandle = nullptr;
            #endif
		};
	}
}

#endif