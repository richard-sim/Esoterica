#pragma once
#ifdef EE_VULKAN

#include "System/Types/Arrays.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		class VulkanInstance
		{
		public:

			struct InitConfig
			{
				static InitConfig GetDefault();

				TVector<char const*>				m_requiredLayers;
				TVector<char const*>				m_requiredExtensions;
				bool								m_enableDebug;
			};

			struct CollectedInfo
			{
				TVector<VkLayerProperties>			m_instanceLayerProps;
				TVector<VkExtensionProperties>		m_instanceExtensionProps;
			};

			VulkanInstance();
			VulkanInstance(InitConfig config);

			~VulkanInstance();

			//-------------------------------------------------------------------------



			//-------------------------------------------------------------------------

			inline TVector<VkLayerProperties>const& GetLayerProps() { return m_collectInfos.m_instanceLayerProps; }
			inline TVector<VkExtensionProperties>const& GetExtensionProps() { return m_collectInfos.m_instanceExtensionProps; }
		private:

			bool CheckAndCollectInstanceLayerProps( InitConfig const& config );
			bool CheckAndCollectInstanceExtensionProps( InitConfig const& config );
			void CreateInstance( InitConfig const& config );

		private:
			
			VkInstance								m_pHandle;
			CollectedInfo							m_collectInfos;
		};
	}
}

#endif // EE_VULKAN