#pragma once
#ifdef EE_VULKAN

#include "Base/Types/Arrays.h"

#define VK_SUCCEEDED(func) do { if ( ((VkResult) func) != VK_SUCCESS ) { EE_TRACE_MSG( "Vulkan Failed At: (" EE_FILE_LINE ")" ); EE_HALT(); } } while(0)

namespace EE::Render
{
	namespace Backend
	{
		TVector<char const*> GetEngineVulkanInstanceRequiredLayers( bool enableDebug );
		TVector<char const*> GetEngineVulkanInstanceRequiredExtensions();

		TVector<char const*> GetEngineVulkanDeviceRequiredLayers( bool enableDebug );
		TVector<char const*> GetEngineVulkanDeviceRequiredExtensions();
	}
}

#endif