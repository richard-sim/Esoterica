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
		class VulkanSurface;

		struct QueueFamily
		{
			QueueFamily() = default;
			QueueFamily( uint32_t index, VkQueueFamilyProperties props )
				: m_index( index ), m_props( props )
			{}

			inline bool IsGraphicQueue() const
			{
				return ( m_props.queueFlags & VK_QUEUE_GRAPHICS_BIT ) != 0;
			}

			inline bool IsTransferQueue() const
			{
				return ( m_props.queueFlags & VK_QUEUE_TRANSFER_BIT ) != 0;
			}

			inline bool IsComputeQueue() const
			{
				return ( m_props.queueFlags & VK_QUEUE_COMPUTE_BIT ) != 0;
			}

			inline bool IsValid() const
			{
				return m_index != std::numeric_limits<uint32_t>::max();
			}

		public:

			uint32_t						m_index = std::numeric_limits<uint32_t>::max();
			VkQueueFamilyProperties			m_props;
		};

		class VulkanPhysicalDevice
		{
		public:

			typedef int GPUPickScore;

			VulkanPhysicalDevice() = default;
			VulkanPhysicalDevice( VkPhysicalDevice pHandle );

			VulkanPhysicalDevice( VulkanPhysicalDevice const& ) = delete;
			VulkanPhysicalDevice& operator=( VulkanPhysicalDevice const& ) = delete;

			VulkanPhysicalDevice( VulkanPhysicalDevice&& ) = default;
			VulkanPhysicalDevice& operator=( VulkanPhysicalDevice&& ) = default;

		private:

			// Calculate pick score for itself, higher the pick score, better the chance VulkanDevice will use it as underlying render device.
			void CalculatePickScore( TSharedPtr<VulkanSurface> const& pSurface );

		private:

			friend class VulkanDevice;
			friend class VulkanSwapchain;

			VkPhysicalDevice					m_pHandle = nullptr;

		public:

			GPUPickScore						m_pickScore = -1;

			TVector<QueueFamily>				m_queueFamilies;
			
			VkPhysicalDeviceFeatures			m_features;
			VkPhysicalDeviceProperties			m_props;
			VkPhysicalDeviceMemoryProperties	m_memoryProps;
		};

		// Pick a most suitable physical device for rendering.
		// Note: Since VulkanPhysicalDevice is uncopyable but movable, this pick operation will erased the picked one in pdDevices.
		// (i.e. `move` the most suitable one out of the pdDevices )
		VulkanPhysicalDevice PickMostSuitablePhysicalDevice( TVector<VulkanPhysicalDevice>& pdDevices );
	}
}

#endif