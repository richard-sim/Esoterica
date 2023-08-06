#ifdef EE_VULKAN
#include "VulkanDevice.h"
#include "VulkanCommonSettings.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanShader.h"
#include "VulkanSemaphore.h"
#include "Base/Logging/Log.h"

namespace EE::Render
{
	namespace Backend
	{
		VulkanQueue::VulkanQueue( VulkanDevice const& device, QueueFamily const& queueFamily )
			: m_queueFamily( queueFamily )
		{
			vkGetDeviceQueue( device.m_pHandle, queueFamily.m_index, 0, &m_pHandle );

			if ( queueFamily.IsGraphicQueue() )
			{
				m_type = Type::Graphic;
			}
			else if ( queueFamily.IsComputeQueue() )
			{
				m_type = Type::Compute;
			}
			else if ( queueFamily.IsTransferQueue() )
			{
				m_type = Type::Transfer;
			}

			EE_ASSERT( m_pHandle != nullptr );
		}

		//-------------------------------------------------------------------------

		VulkanDevice::InitConfig VulkanDevice::InitConfig::GetDefault( bool enableDebug )
		{
			InitConfig config;
			config.m_requiredLayers = GetEngineVulkanDeviceRequiredLayers( enableDebug );
			config.m_requiredExtensions = GetEngineVulkanDeviceRequiredExtensions();
			return config;
		}

		//-------------------------------------------------------------------------

		VulkanDevice::VulkanDevice()
		{
            m_pInstance = MakeShared<VulkanInstance>();
            EE_ASSERT( m_pInstance != nullptr );

            m_pSurface = MakeShared<VulkanSurface>( m_pInstance );
            EE_ASSERT( m_pSurface != nullptr );

            InitConfig const config = InitConfig::GetDefault( m_pInstance->IsEnableDebug() );

            PickPhysicalDeviceAndCreate( config );
        }

		VulkanDevice::VulkanDevice( InitConfig config )
		{
            m_pInstance = MakeShared<VulkanInstance>();
            EE_ASSERT( m_pInstance != nullptr );

            m_pSurface = MakeShared<VulkanSurface>( m_pInstance );
            EE_ASSERT( m_pSurface != nullptr );

            PickPhysicalDeviceAndCreate( config );
		}

		VulkanDevice::~VulkanDevice()
		{
			EE_ASSERT( m_pHandle != nullptr );

			vkDestroyDevice( m_pHandle, nullptr );
			m_pHandle = nullptr;
		}

		//-------------------------------------------------------------------------

        RHI::RHITexture* VulkanDevice::CreateTexture( RHI::RHITextureCreateDesc const& createDesc )
        {
            return nullptr;
        }

        void VulkanDevice::DestroyTexture( RHI::RHITexture* pTexture )
        {

        }

        RHI::RHISemaphore* VulkanDevice::CreateSyncSemaphore( RHI::RHISemaphoreCreateDesc const& createDesc )
        {
            EE_ASSERT( createDesc.IsValid() );

            VulkanSemaphore* pVkSemaphore = EE::New<VulkanSemaphore>();
            VkSemaphoreCreateInfo semaphoreCI = {};
            semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreCI.pNext = nullptr;
            semaphoreCI.flags = VkFlags( 0 );

            VK_SUCCEEDED( vkCreateSemaphore( m_pHandle, &semaphoreCI, nullptr, &(pVkSemaphore->m_pHandle) ) );

            return pVkSemaphore;
        }

        void VulkanDevice::DestroySyncSemaphore( RHI::RHISemaphore* pShader )
        {
            EE_ASSERT( pShader != nullptr );
            VulkanSemaphore* pVkSemaphore = static_cast<VulkanSemaphore*>( pShader );
            EE_ASSERT( pVkSemaphore->m_pHandle != nullptr );

            vkDestroySemaphore( m_pHandle, pVkSemaphore->m_pHandle, nullptr );
            
            EE::Delete( pVkSemaphore );
        }

        RHI::RHIShader* VulkanDevice::CreateShader( RHI::RHIShaderCreateDesc const& createDesc )
        {
            EE_ASSERT( createDesc.IsValid() );

            VulkanShader* pVkShader = EE::New<VulkanShader>();
            VkShaderModuleCreateInfo shaderModuleCI = {};
            shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleCI.pCode = reinterpret_cast<uint32_t const*>( createDesc.m_byteCode.data() );
            shaderModuleCI.codeSize = createDesc.m_byteCode.size(); // Note: here is the size in bytes!!

            VK_SUCCEEDED( vkCreateShaderModule( m_pHandle, &shaderModuleCI, nullptr, &(pVkShader->m_pModule) ) );

            return pVkShader;
        }

		void VulkanDevice::DestroyShader( RHI::RHIShader* pResource )
		{
            EE_ASSERT( pResource != nullptr );
            VulkanShader* pVkShader = static_cast<VulkanShader*>( pResource );
            EE_ASSERT( pVkShader->m_pModule != nullptr );

            vkDestroyShaderModule( m_pHandle, pVkShader->m_pModule, nullptr );

            EE::Delete( pVkShader );
		}

		//-------------------------------------------------------------------------

        void VulkanDevice::PickPhysicalDeviceAndCreate( InitConfig const& config )
        {
            auto pdDevices = m_pInstance->EnumeratePhysicalDevice();

            for ( auto& pd : pdDevices )
            {
                pd.CalculatePickScore( m_pSurface );
            }

            auto physicalDevice = PickMostSuitablePhysicalDevice( pdDevices );
            m_physicalDevice = std::move( physicalDevice );

            EE_ASSERT( CheckAndCollectDeviceLayers( config ) );
            EE_ASSERT( CheckAndCollectDeviceExtensions( config ) );

            EE_ASSERT( CreateDevice( config ) );
        }

		bool VulkanDevice::CheckAndCollectDeviceLayers( InitConfig const& config )
		{
			uint32_t layerCount = 0;
			VK_SUCCEEDED( vkEnumerateDeviceLayerProperties( m_physicalDevice.m_pHandle, &layerCount, nullptr ) );

			EE_ASSERT( layerCount > 0 );

			TVector<VkLayerProperties> layerProps( layerCount );
			VK_SUCCEEDED( vkEnumerateDeviceLayerProperties( m_physicalDevice.m_pHandle, &layerCount, layerProps.data() ) );
			m_collectInfos.m_deviceLayerProps = layerProps;

			for ( auto const& required : config.m_requiredLayers )
			{
				bool foundLayer = false;

				for ( auto const& layer : layerProps )
				{
					if ( strcmp( required, layer.layerName ) == 0 )
					{
						foundLayer = true;
						break;
					}
				}

				if ( !foundLayer )
				{
					EE_LOG_ERROR( "Render", "Vulkan Backend", "Device layer not found: %s", required );
					return false;
				}
			}

			return true;
		}

		bool VulkanDevice::CheckAndCollectDeviceExtensions( InitConfig const& config )
		{
			uint32_t extCount = 0;
			VK_SUCCEEDED( vkEnumerateDeviceExtensionProperties( m_physicalDevice.m_pHandle, nullptr, &extCount, nullptr ) );

			EE_ASSERT( extCount > 0 );

			TVector<VkExtensionProperties> extProps( extCount );
			VK_SUCCEEDED( vkEnumerateDeviceExtensionProperties( m_physicalDevice.m_pHandle, nullptr, &extCount, extProps.data() ) );
			m_collectInfos.m_deviceExtensionProps = extProps;

			for ( auto const& required : config.m_requiredExtensions )
			{
				bool foundExt = false;

				for ( auto const& ext : extProps )
				{
					if ( strcmp( required, ext.extensionName ) == 0 )
					{
						foundExt = true;
						break;
					}
				}

				if ( !foundExt )
				{
					EE_LOG_ERROR( "Render", "Vulkan Backend", "Device extension not found: %s", required );
					return false;
				}
			}

			return true;
		}

		bool VulkanDevice::CreateDevice( InitConfig const& config )
		{
			// device queue creation info population
			//-------------------------------------------------------------------------

			TVector<VkDeviceQueueCreateInfo> deviceQueueCIs = {};
			TVector<QueueFamily> deviceQueueFamilies = {};

			float priorities[] = { 1.0f };

			// only create one graphic queue for now
			for ( auto const& qf : m_physicalDevice.m_queueFamilies )
			{
				if ( qf.IsGraphicQueue() )
				{
					VkDeviceQueueCreateInfo dqCI = {};
					dqCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					dqCI.flags = VkFlags( 0 );
					dqCI.pNext = nullptr;

					dqCI.queueCount = 1;
					dqCI.queueFamilyIndex = qf.m_index;
					dqCI.pQueuePriorities = priorities;

					deviceQueueCIs.push_back( dqCI );
					deviceQueueFamilies.push_back( qf );
					break;
				}
			}

			if ( deviceQueueCIs.empty() )
			{
				EE_LOG_ERROR( "Render", "Vulkan Backend", "Invalid physical device which not supports graphic queue!" );
				return false;
			}

			// physical device features2 validation
			//-------------------------------------------------------------------------

			auto descriptor_indexing = VkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
			descriptor_indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

			auto imageless_framebuffer = VkPhysicalDeviceImagelessFramebufferFeaturesKHR{};
			imageless_framebuffer.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR;

			auto buffer_address = VkPhysicalDeviceBufferDeviceAddressFeaturesEXT{};
			buffer_address.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT;

			// TODO: pNext chain
			VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
			physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

			physicalDeviceFeatures2.pNext = &descriptor_indexing;
			descriptor_indexing.pNext = &imageless_framebuffer;
			imageless_framebuffer.pNext = &buffer_address;

			vkGetPhysicalDeviceFeatures2( m_physicalDevice.m_pHandle, &physicalDeviceFeatures2 );

			EE_ASSERT( imageless_framebuffer.imagelessFramebuffer );
			EE_ASSERT( descriptor_indexing.descriptorBindingPartiallyBound );
			EE_ASSERT( buffer_address.bufferDeviceAddress );

			// device creation
			//-------------------------------------------------------------------------

			VkDeviceCreateInfo deviceCI = {};
			deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCI.flags = VkFlags( 0 );
			deviceCI.pNext = &physicalDeviceFeatures2;

			deviceCI.pQueueCreateInfos = deviceQueueCIs.data();
			deviceCI.queueCreateInfoCount = static_cast<uint32_t>( deviceQueueCIs.size() );

			deviceCI.ppEnabledLayerNames = config.m_requiredLayers.data();
			deviceCI.enabledLayerCount = static_cast<uint32_t>( config.m_requiredLayers.size() );
			deviceCI.ppEnabledExtensionNames = config.m_requiredExtensions.data();
			deviceCI.enabledExtensionCount = static_cast<uint32_t>( config.m_requiredExtensions.size() );

			VK_SUCCEEDED( vkCreateDevice( m_physicalDevice.m_pHandle, &deviceCI, nullptr, &m_pHandle ) );

			// fetch global device queue
			//-------------------------------------------------------------------------

			m_globalGraphicQueue = VulkanQueue( *this, deviceQueueFamilies[0] );

			return true;
		}
	}
}

#endif