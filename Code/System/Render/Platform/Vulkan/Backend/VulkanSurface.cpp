#ifdef EE_VULKAN

#include "VulkanSurface.h"
#include "VulkanCommonSettings.h"
#include "VulkanInstance.h"

namespace EE::Render
{
	namespace Backend
	{
		VulkanSurface::VulkanSurface( TSharedPtr<VulkanInstance> pInstance )
			: m_pInstance ( pInstance )
		{
			LoadVulkanFuncPointer();

			EE_ASSERT( m_loadFunc.m_pCreateFunc != nullptr );

			#ifdef _WIN32
			EE_ASSERT( CreateWin32Surface() );
			#endif
		}

		VulkanSurface::~VulkanSurface()
		{
			EE_ASSERT( m_loadFunc.m_pDestroyFunc != nullptr );

			m_loadFunc.m_pDestroyFunc( m_pInstance->m_pHandle, m_pHandle, nullptr );
			m_pHandle = nullptr;
		}

		//-------------------------------------------------------------------------

		void VulkanSurface::LoadVulkanFuncPointer()
		{
			EE_ASSERT( m_pInstance );

			m_loadFunc.m_pCreateFunc = (PFN_vkCreateWin32SurfaceKHR) m_pInstance->GetProcAddress( "vkCreateWin32SurfaceKHR" );
			m_loadFunc.m_pDestroyFunc = (PFN_vkDestroySurfaceKHR) m_pInstance->GetProcAddress( "vkDestroySurfaceKHR" );
			m_loadFunc.m_pGetPhysicalDeviceSupport = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) m_pInstance->GetProcAddress( "vkGetPhysicalDeviceSurfaceSupportKHR" );
		}

		bool VulkanSurface::CreateWin32Surface()
		{
			HWND hwnd = GetActiveWindow();

			if ( hwnd == nullptr )
			{
				return false;
			}

			HINSTANCE hinstance = (HINSTANCE) GetWindowLongPtr( hwnd, GWLP_HINSTANCE );

			if ( hinstance == nullptr )
			{
				return false;
			}

			VkWin32SurfaceCreateInfoKHR win32SurfaceCI = {};
			win32SurfaceCI.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			win32SurfaceCI.flags = VkFlags( 0 );
			win32SurfaceCI.hinstance = hinstance;
			win32SurfaceCI.hwnd = hwnd;
			win32SurfaceCI.pNext = nullptr;

			VK_SUCCEEDED( m_loadFunc.m_pCreateFunc( m_pInstance->m_pHandle, &win32SurfaceCI, nullptr, &m_pHandle ) );

			return true;
		}

	}
}

#endif