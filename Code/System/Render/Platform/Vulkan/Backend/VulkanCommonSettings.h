#pragma once
#ifdef EE_VULKAN

#include "System/Types/Arrays.h"

#include <vulkan/vulkan_core.h>

#define VK_SUCCEEDED(func) do { if ( ((VkResult) func) != VK_SUCCESS ) { EE_TRACE_MSG( "Vulkan Failed At: (" EE_FILE_LINE ")" ); EE_HALT(); } } while(0)

namespace EE::Render
{
	namespace Backend
	{
		TVector<char const*> GetVulkanInstanceRequiredLayers();
		TVector<char const*> GetVulkanInstanceRequiredExtensions();
	}
}

#endif