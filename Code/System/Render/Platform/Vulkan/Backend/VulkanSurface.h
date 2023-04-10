#pragma once
#ifdef EE_VULKAN

#include "System/Memory/Pointers.h"

#include <vulkan/vulkan_core.h>

#ifdef _WIN32
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace EE::Render
{
	namespace Backend
	{
		class VulkanInstance;

		class VulkanSurface
		{
		public:

			struct LoadFunc
			{
				#ifdef _WIN32
				PFN_vkCreateWin32SurfaceKHR					m_pCreateFunc = nullptr;
				#endif

				PFN_vkDestroySurfaceKHR						m_pDestroyFunc = nullptr;
				PFN_vkGetPhysicalDeviceSurfaceSupportKHR	m_pGetPhysicalDeviceSupport = nullptr;
			};

		public:

			VulkanSurface( TSharedPtr<VulkanInstance> pInstance );
			~VulkanSurface();

		private:

			void LoadVulkanFuncPointer();

			#ifdef _WIN32
			bool CreateWin32Surface();
			#else
			#error No surface create function!
			#endif

		private:

			friend class VulkanPhysicalDevice;

			TSharedPtr<VulkanInstance>				m_pInstance = nullptr;
			LoadFunc								m_loadFunc;

			VkSurfaceKHR							m_pHandle = nullptr;
		};
	}
}

#endif // EE_VULKAN