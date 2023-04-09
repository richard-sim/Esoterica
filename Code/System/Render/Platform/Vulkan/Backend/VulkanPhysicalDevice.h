#pragma once
#ifdef EE_VULKAN

#include "System/Types/Arrays.h"
#include "System/Memory/Pointers.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanInstance;

		class VulkanPhysicalDevice
		{
		public:

			struct QueueFamily
			{
				uint32_t						m_index;
				VkQueueFamilyProperties			m_props;
			};

			//-------------------------------------------------------------------------

			VulkanPhysicalDevice( TSharedPtr<VulkanInstance> instance );

			//-------------------------------------------------------------------------

		private:

			VkPhysicalDevice					m_pHandle;
			TSharedPtr<VulkanInstance>			m_pInstance; // keep a reference to vulkan instance 

			TVector<QueueFamily>				m_queueFamilies;
			
			VkPhysicalDeviceFeatures			m_features;
			VkPhysicalDeviceProperties			m_props;
			VkPhysicalDeviceMemoryProperties	m_memoryProps;
		};
	}
}

#endif