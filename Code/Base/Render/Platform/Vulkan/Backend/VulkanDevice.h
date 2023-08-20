#pragma once
#if defined(EE_VULKAN)

#include "Base/Memory/Pointers.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanResource.h"
#include "VulkanMemoryAllocator.h"

#include "Base/RHI/RHIDevice.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"

#include <vulkan/vulkan_core.h>

namespace EE::RHI
{
    class RHIShader;
    class RHIBuffer;
    class RHITexture;
    class RHISemaphore;
}

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

		class VulkanDevice final : public RHI::RHIDevice
		{
            friend class VulkanMemoryAllocator;
            friend class VulkanQueue;
            friend class VulkanSwapchain;

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
			VulkanDevice();
			VulkanDevice( InitConfig config );

			~VulkanDevice();

		public:

            virtual RHI::RHITexture* CreateTexture( RHI::RHITextureCreateDesc const& createDesc ) override;
            virtual void             DestroyTexture( RHI::RHITexture* pTexture ) override;

            virtual RHI::RHIBuffer* CreateBuffer( RHI::RHIBufferCreateDesc const& createDesc ) override;
            virtual void            DestroyBuffer( RHI::RHIBuffer* pBuffer ) override;

            virtual RHI::RHISemaphore* CreateSyncSemaphore( RHI::RHISemaphoreCreateDesc const& createDesc ) override;
            virtual void DestroySyncSemaphore( RHI::RHISemaphore* pSemaphore ) override;

            //-------------------------------------------------------------------------

            virtual RHI::RHIShader* CreateShader( RHI::RHIShaderCreateDesc const& createDesc ) override;
            virtual void            DestroyShader( RHI::RHIShader* pShader ) override;

		private:

            void PickPhysicalDeviceAndCreate( InitConfig const& config );
			bool CheckAndCollectDeviceLayers( InitConfig const& config );
			bool CheckAndCollectDeviceExtensions( InitConfig const& config );
			bool CreateDevice( InitConfig const& config );

            // Utility functions
            //-------------------------------------------------------------------------
		
            bool VulkanDevice::GetMemoryType( uint32_t typeBits, VkMemoryPropertyFlags properties, uint32_t& memTypeFound ) const;

		private:

			TSharedPtr<VulkanInstance>			    m_pInstance = nullptr;
            TSharedPtr<VulkanSurface>               m_pSurface = nullptr;

			VkDevice							    m_pHandle = nullptr;
			CollectedInfo						    m_collectInfos;
			VulkanPhysicalDevice				    m_physicalDevice;

			VulkanQueue							    m_globalGraphicQueue;
            VulkanMemoryAllocator                   m_globalMemoryAllcator;
		};
	}
}

#endif