#ifdef EE_VULKAN

#include "VulkanInstance.h"
#include "VulkanCommonSettings.h"
#include "System/Memory/Memory.h"
#include "System/Log.h"

namespace EE::Render
{
	namespace Backend
	{
		static VkBool32 DebugUtilsUserCallback( VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
												VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
												const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
												void* pUserData )
		{
			char const* messageTypeStr = "";
			switch ( messageTypes )
			{
				case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: messageTypeStr = "[General]"; break;
				case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: messageTypeStr = "[Validation]"; break;
				case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: messageTypeStr = "[Performance]"; break;
				case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT: messageTypeStr = "[DeviceAddessBinding]"; break;
				default: messageTypeStr = "[Unknown]"; break;
			}

			switch ( messageSeverity )
			{
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
					EE_LOG_MESSAGE( "Render", "Vulkan Backend", "%s %s", messageTypeStr, pCallbackData->pMessage );
				break;

				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
					EE_LOG_MESSAGE( "Render", "Vulkan Backend", "%s %s", messageTypeStr, pCallbackData->pMessage );
				break;

				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
					EE_LOG_WARNING( "Render", "Vulkan Backend", "%s %s", messageTypeStr, pCallbackData->pMessage );
				break;

				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
					EE_LOG_ERROR( "Render", "Vulkan Backend", "%s %s", messageTypeStr, pCallbackData->pMessage );
				break;

				default:
					EE_UNREACHABLE_CODE();
				break;
			}

			return false;
		}

		static VkDebugUtilsMessengerCreateInfoEXT PopulateDebugMessageerCreateInfo()
		{
			VkDebugUtilsMessengerCreateInfoEXT info = {};
			info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			info.pNext = nullptr;
			info.flags = VkFlags( 0 );
			info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
			info.pfnUserCallback = DebugUtilsUserCallback;
			info.pUserData = nullptr;
			return info;
		}

		//-------------------------------------------------------------------------

		VulkanInstance::InitConfig VulkanInstance::InitConfig::GetDefault()
		{
			InitConfig config;
			config.m_requiredLayers = GetVulkanInstanceRequiredLayers();
			config.m_requiredExtensions = GetVulkanInstanceRequiredExtensions();
			#ifdef EE_DEBUG
			config.m_enableDebug = true;
			#else
			config.m_enableDebug = false;
			#endif

			return std::move(config);
		}

		//-------------------------------------------------------------------------

		VulkanInstance::VulkanInstance()
			: VulkanInstance( InitConfig::GetDefault() )
		{}

		VulkanInstance::VulkanInstance( InitConfig config )
		{
			// TODO: use volk
			EE_ASSERT( CheckAndCollectInstanceLayerProps(config) );
			EE_ASSERT( CheckAndCollectInstanceExtensionProps(config) );

			CreateInstance( config );
		}

		VulkanInstance::~VulkanInstance()
		{
			vkDestroyInstance( m_pHandle, nullptr );
		}

		//-------------------------------------------------------------------------

		bool VulkanInstance::CheckAndCollectInstanceLayerProps( InitConfig const& config )
		{
			uint32_t layerCount = 0;
			VK_SUCCEEDED( vkEnumerateInstanceLayerProperties( &layerCount, nullptr ) );

			EE_ASSERT( layerCount > 0 );

			TVector<VkLayerProperties> layerProps( layerCount );
			VK_SUCCEEDED( vkEnumerateInstanceLayerProperties( &layerCount, layerProps.data() ) );
			m_collectInfos.m_instanceLayerProps = layerProps;
			
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
					EE_LOG_ERROR( "Render", "Vulkan Backend", "Instance layer not found: %s", required );
					return false;
				}
			}

			return true;
		}

		bool VulkanInstance::CheckAndCollectInstanceExtensionProps( InitConfig const& config )
		{
			uint32_t extCount = 0;
			VK_SUCCEEDED( vkEnumerateInstanceExtensionProperties( nullptr, &extCount, nullptr ) );

			EE_ASSERT( extCount > 0 );

			TVector<VkExtensionProperties> extProps( extCount );
			VK_SUCCEEDED( vkEnumerateInstanceExtensionProperties( nullptr, &extCount, extProps.data() ) );
			m_collectInfos.m_instanceExtensionProps = extProps;

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
					EE_LOG_ERROR( "Render", "Vulkan Backend", "Instance extension not found: %s", required );
					return false;
				}
			}

			return true;
		}

		void VulkanInstance::CreateInstance( InitConfig const& config )
		{
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = nullptr;
			appInfo.pEngineName = "Esoterica Engine";
			appInfo.pApplicationName = "Unknown";
			appInfo.apiVersion = VK_API_VERSION_1_3;
			appInfo.applicationVersion = VK_MAKE_API_VERSION( 0, 0, 1, 0 );
			appInfo.engineVersion = VK_MAKE_API_VERSION( 0, 0, 1, 0 );

			VkInstanceCreateInfo instanceCI = {};
			instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCI.pNext = nullptr;
			instanceCI.flags = VkFlags( 0 );
			instanceCI.pApplicationInfo = &appInfo;

			auto debugMessagerCreateInfo = PopulateDebugMessageerCreateInfo();

			if ( config.m_enableDebug )
			{
				instanceCI.pNext = (void*) &debugMessagerCreateInfo;
			}

			instanceCI.enabledLayerCount = 0;
			instanceCI.ppEnabledLayerNames = nullptr;
			instanceCI.enabledExtensionCount = 0;
			instanceCI.ppEnabledExtensionNames = nullptr;

			VK_SUCCEEDED( vkCreateInstance( &instanceCI, nullptr, &m_pHandle ) );
		}
	}
}

#endif // EE_VULKAN