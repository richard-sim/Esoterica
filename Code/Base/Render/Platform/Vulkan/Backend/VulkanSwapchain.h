#pragma once
#ifdef EE_VULKAN

#include "Base/Memory/Pointers.h"
#include "Base/Types/Arrays.h"
#include "VulkanResource.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanSurface;
		class VulkanDevice;

		class VulkanSwapchain
		{
		public:

			struct InitConfig
			{
				static InitConfig GetDefault();

				bool				m_enableVsync;
				uint32_t			m_width;
				uint32_t			m_height;
			};

			struct LoadFuncs
			{
				PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR		m_pGetPhysicalDeviceSurfaceCapabilitiesKHRFunc;
				PFN_vkGetPhysicalDeviceSurfaceFormatsKHR			m_pGetPhysicalDeviceSurfaceFormatsKHRFunc;
				PFN_vkGetPhysicalDeviceSurfacePresentModesKHR		m_pGetPhysicalDeviceSurfacePresentModesKHRFunc;
				PFN_vkCreateSwapchainKHR							m_pCreateSwapchainKHRFunc;
				PFN_vkDestroySwapchainKHR							m_pDestroySwapchainKHRFunc;
				PFN_vkGetSwapchainImagesKHR							m_pGetSwapchainImagesKHRFunc;
			};

		public:

			VulkanSwapchain( TSharedPtr<VulkanSurface> pSurface, TSharedPtr<VulkanDevice> pDevice );
			VulkanSwapchain( InitConfig config, TSharedPtr<VulkanSurface> pSurface, TSharedPtr<VulkanDevice> pDevice );

			VulkanSwapchain( VulkanSwapchain const& ) = delete;
			VulkanSwapchain& operator=( VulkanSwapchain const& ) = delete;

			VulkanSwapchain( VulkanSwapchain&& ) = default;
			VulkanSwapchain& operator=( VulkanSwapchain&& ) = default;

			~VulkanSwapchain();

		private:

			TSharedPtr<VulkanSurface>				m_pSurface;
			TSharedPtr<VulkanDevice>				m_pDevice;

			VkSwapchainKHR							m_pHandle;
			LoadFuncs								m_loadFuncs;

			TVector<VulkanTexture>					m_images;
			TVector<VulkanSemaphore>				m_imageAcquireSemaphores;
			TVector<VulkanSemaphore>				m_renderCompleteSemaphores;
		};
	}
}

#endif