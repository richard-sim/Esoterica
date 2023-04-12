#pragma once
#ifdef EE_VULKAN

#include "System/Memory/Pointers.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanTexture.h"
#include "VulkanSemaphore.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanInstance;
		class VulkanSurface;

		class VulkanQueue
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

			VulkanQueue() = default;
			VulkanQueue( VulkanDevice const& device, QueueFamily const& queueFamily );

			VulkanQueue( VulkanQueue const& ) = delete;
			VulkanQueue& operator=( VulkanQueue const& ) = delete;

			VulkanQueue( VulkanQueue&& ) = default;
			VulkanQueue& operator=( VulkanQueue&& ) = default;

		public:

			inline bool IsValid() const { return m_pHandle != nullptr; }

		private:

			VkQueue								m_pHandle = nullptr;
			Type								m_type = Type::Unknown;
			QueueFamily							m_queueFamily;
		};

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

		public:

			// Only support single physical device for now.
			VulkanDevice( TSharedPtr<VulkanInstance> pInstance, TSharedPtr<VulkanSurface> pSurface );
			VulkanDevice( InitConfig config, TSharedPtr<VulkanInstance> pInstance, TSharedPtr<VulkanSurface> pSurface );

			VulkanDevice( VulkanDevice const& ) = delete;
			VulkanDevice& operator=( VulkanDevice const& ) = delete;

			VulkanDevice( VulkanDevice&& ) = default;
			VulkanDevice& operator=( VulkanDevice&& ) = default;

			~VulkanDevice();

		public:

			VulkanTexture CreateTexture( TextureDesc desc );
			void DestroyTexture( VulkanTexture texture );

			VulkanSemaphore CreateVSemaphore();
			void DestroyVSemaphore( VulkanSemaphore semaphore );

		private:

			bool CheckAndCollectDeviceLayers( InitConfig const& config );
			bool CheckAndCollectDeviceExtensions( InitConfig const& config );
			bool CreateDevice( InitConfig const& config );
		
		private:

			friend class VulkanQueue;
			friend class VulkanSwapchain;

			TSharedPtr<VulkanInstance>			m_pInstance;

			VkDevice							m_pHandle = nullptr;
			CollectedInfo						m_collectInfos;
			VulkanPhysicalDevice				m_physicalDevice;

			VulkanQueue							m_globalGraphicQueue;
		};
	}
}

#endif