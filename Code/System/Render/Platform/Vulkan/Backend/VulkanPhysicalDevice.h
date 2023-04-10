#pragma once
#ifdef EE_VULKAN

#include "System/Types/Arrays.h"
#include "System/Memory/Pointers.h"
#include "VulkanQueue.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanInstance;
		class VulkanSurface;

		class VulkanPhysicalDevice
		{
		public:

			typedef int GPUPickScore;

			struct QueueFamily
			{
				QueueFamily( uint32_t index, VkQueueFamilyProperties props )
					: m_index( index ), m_props( props )
				{}

				inline bool IsGraphicQueue() const { return ( m_props.queueFlags & VK_QUEUE_GRAPHICS_BIT ) != 0; }
				inline bool IsTransferQueue() const { return ( m_props.queueFlags & VK_QUEUE_TRANSFER_BIT ) != 0; }
				inline bool IsComputeQueue() const { return ( m_props.queueFlags & VK_QUEUE_COMPUTE_BIT ) != 0; }

			public:

				uint32_t						m_index;
				VkQueueFamilyProperties			m_props;
			};

			VulkanPhysicalDevice( VkPhysicalDevice pHandle );

			// Calculate pick score for itself, higher the pick score, better the chance VulkanDevice will use it as underlying render device.
			void CalculatePickScore( TSharedPtr<VulkanSurface> const& pSurface );

		private:

			friend class VulkanDevice;

			VkPhysicalDevice					m_pHandle;

		public:

			GPUPickScore						m_pickScore = -1;

			TVector<QueueFamily>				m_queueFamilies;
			
			VkPhysicalDeviceFeatures			m_features;
			VkPhysicalDeviceProperties			m_props;
			VkPhysicalDeviceMemoryProperties	m_memoryProps;
		};

		VulkanPhysicalDevice PickMostSuitablePhysicalDevice( TVector<VulkanPhysicalDevice> const& pdDevices );
	}
}

#endif