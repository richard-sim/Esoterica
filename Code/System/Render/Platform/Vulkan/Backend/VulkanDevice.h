#pragma once
#ifdef EE_VULKAN

#include "System/Memory/Pointers.h"
#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanInstance;

		class VulkanDevice
		{
		public:

			struct InitConfig
			{
				static InitConfig GetDefault( bool enableDebug );

				TVector<char const*>				m_requiredLayers;
				TVector<char const*>				m_requiredExtensions;
			};

			struct CollectedInfo
			{
				TVector<VkLayerProperties>			m_deviceLayerProps;
				TVector<VkExtensionProperties>		m_deviceExtensionProps;
			};

			// Only support single physical device for now.
			VulkanDevice( TSharedPtr<VulkanInstance> pInstance, VulkanPhysicalDevice pdDevice );
			VulkanDevice( InitConfig config, TSharedPtr<VulkanInstance> pInstance, VulkanPhysicalDevice pdDevice );

			~VulkanDevice();

		private:

			bool CheckAndCollectDeviceLayers( InitConfig const& config );
			bool CheckAndCollectDeviceExtensions( InitConfig const& config );
			bool CreateDevice( InitConfig const& config );
		
		private:

			TSharedPtr<VulkanInstance>			m_pInstance;

			VkDevice							m_pHandle = nullptr;
			CollectedInfo						m_collectInfos;
			VulkanPhysicalDevice				m_physicalDevice;
		};
	}
}

#endif