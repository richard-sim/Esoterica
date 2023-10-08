#pragma once
#if defined(EE_VULKAN)

#include "Base/Types/Arrays.h"
#include "Base/Application/Application.h"

#include "Base/RHI/RHISwapchain.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanDevice;
        class VulkanTexture;
        class VulkanSemaphore;

		class VulkanSwapchain final : public RHI::RHISwapchain
		{
		public:

			struct InitConfig
			{
				static InitConfig GetDefault( Application* pApplication );

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

			VulkanSwapchain( Application* pApplication, VulkanDevice* pDevice );
			VulkanSwapchain( InitConfig config, Application* pApplication, VulkanDevice* pDevice );

			~VulkanSwapchain();

		private:

            Application*                            m_pApplication = nullptr;

		    // Cache device pointer (Use TSharedPtr ?)
			VulkanDevice*           				m_pDevice = nullptr;

			VkSwapchainKHR							m_pHandle;
			LoadFuncs								m_loadFuncs;

			TVector<VulkanTexture*>					m_images;
			TVector<VulkanSemaphore*>				m_imageAcquireSemaphores;
			TVector<VulkanSemaphore*>				m_renderCompleteSemaphores;
		};
	}
}

#endif