#ifdef EE_VULKAN

#include "VulkanCommonSettings.h"
namespace EE::Render
{
	namespace Backend
	{
		#ifdef EE_DEBUG
		static char const* const gEngineRequiredInstanceLayers[] = {
			"VK_LAYER_KHRONOS_validation",
		};
		#endif

		#ifdef _WIN32
		#include <windows.h>
		#include <vulkan/vulkan_win32.h>
		#ifdef EE_DEBUG
		static char const* const gEngineRequiredInstanceExtensions[] = {
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		};
		#else
		static char const* const gEngineRequiredInstanceExtensions[] = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		};
		#endif // EE_DEBUG
		#else // _WIN32
		#error Unsupported vulkan platform!
		#endif

		TVector<char const*> GetVulkanInstanceRequiredLayers()
		{
			#ifdef EE_DEBUG
			TVector<char const*> validationLayers = {};
			size_t const layerCount = sizeof( gEngineRequiredInstanceLayers ) / sizeof( gEngineRequiredInstanceLayers[0] );

			for ( int i = 0; i < layerCount; ++i )
			{
				validationLayers.push_back( gEngineRequiredInstanceLayers[i] );
			}

			return validationLayers;
			#else
			return TVector<char const*> {};
			#endif
		}

		TVector<char const*> GetVulkanInstanceRequiredExtensions()
		{
			TVector<char const*> validationExtensions = {};
			size_t const extCount = sizeof( gEngineRequiredInstanceExtensions ) / sizeof( gEngineRequiredInstanceExtensions[0] );

			for ( int i = 0; i < extCount; ++i )
			{
				validationExtensions.push_back( gEngineRequiredInstanceExtensions[i] );
			}

			return validationExtensions;
		}
	}
}

#endif // EE_VULKAN