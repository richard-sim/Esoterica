#pragma once
#if defined(EE_VULKAN)

#include "VulkanPhysicalDevice.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanSampler.h"
#include "Base/Types/Map.h"
#include "Base/Types/String.h"
#include "Base/Types/HashMap.h"
#include "Base/Memory/Pointers.h"
#include "Base/Resource/ResourcePtr.h"
#include "Base/RHI/RHIDevice.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"

#include <vulkan/vulkan_core.h>

namespace EE::RHI
{
    class RHIShader;
    class RHIBuffer;
    class RHITexture;
    class RHISemaphore;
}

//-------------------------------------------------------------------------

namespace EE::Render
{
	namespace Backend
	{
		class VulkanInstance;
		class VulkanSurface;
        class VulkanPipelineState;

		class VulkanQueue
		{
		public:

			enum class Type : uint8_t
			{
				Graphic,
				Compute,
				Transfer,
				Unknown
			};

		public:

			VulkanQueue() = default;
			VulkanQueue( VulkanDevice const& device, QueueFamily const& queueFamily );

			VulkanQueue( VulkanQueue const& ) = delete;
			VulkanQueue& operator=( VulkanQueue const& ) = delete;

			VulkanQueue( VulkanQueue&& ) = default;
			VulkanQueue& operator=( VulkanQueue&& ) = default;

		public:

			inline bool IsValid() const { return m_pHandle != nullptr; }

		private:

			VkQueue								m_pHandle = nullptr;
			Type								m_type = Type::Unknown;
			QueueFamily							m_queueFamily;
		};

		class VulkanDevice final : public RHI::RHIDevice
		{
            friend class VulkanMemoryAllocator;
            friend class VulkanQueue;
            friend class VulkanSwapchain;
            friend class VulkanFramebufferCache;

        public:

			struct InitConfig
			{
				static InitConfig GetDefault( bool enableDebug );

				TVector<char const*>				m_requiredLayers;
				TVector<char const*>				m_requiredExtensions;
			};

			struct CollectedInfo
			{
				TVector<VkLayerProperties>			m_deviceLayerProps;
				TVector<VkExtensionProperties>		m_deviceExtensionProps;
			};

        public:

            EE_RHI_STATIC_TAGGED_TYPE( RHI::ERHIType::Vulkan )

			// Only support single physical device for now.
			VulkanDevice();
			VulkanDevice( InitConfig config );

			~VulkanDevice();

		public:

            virtual RHI::RHITexture* CreateTexture( RHI::RHITextureCreateDesc const& createDesc ) override;
            virtual void             DestroyTexture( RHI::RHITexture* pTexture ) override;

            virtual RHI::RHIBuffer* CreateBuffer( RHI::RHIBufferCreateDesc const& createDesc ) override;
            virtual void            DestroyBuffer( RHI::RHIBuffer* pBuffer ) override;

            virtual RHI::RHIShader* CreateShader( RHI::RHIShaderCreateDesc const& createDesc ) override;
            virtual void            DestroyShader( RHI::RHIShader* pShader ) override;

            virtual RHI::RHISemaphore* CreateSyncSemaphore( RHI::RHISemaphoreCreateDesc const& createDesc ) override;
            virtual void               DestroySyncSemaphore( RHI::RHISemaphore* pSemaphore ) override;

            //-------------------------------------------------------------------------

            virtual RHI::RHIRenderPass* CreateRenderPass( RHI::RHIRenderPassCreateDesc const& createDesc) override;
            virtual void                DestroyRenderPass( RHI::RHIRenderPass* pRenderPass ) override;

            //-------------------------------------------------------------------------

            virtual RHI::RHIPipelineState* CreateRasterPipelineState( RHI::RHIRasterPipelineStateCreateDesc const& createDesc, CompiledShaderArray const& compiledShaders ) override;
            virtual void                   DestroyRasterPipelineState( RHI::RHIPipelineState* pPipelineState ) override;

		private:

            using CombinedShaderBindingLayout = TMap<uint32_t, Render::Shader::ResourceBinding>;
            using CombinedShaderSetLayout = TFixedMap<uint32_t, CombinedShaderBindingLayout, Render::Shader::NumMaxResourceBindingSet>;

            struct VulkanDescriptorSetLayoutInfos
            {
                TFixedVector<VkDescriptorSetLayout, Render::Shader::NumMaxResourceBindingSet>                  m_vkDescriptorSetLayouts;
                TFixedVector<TMap<uint32_t, VkDescriptorType>, Render::Shader::NumMaxResourceBindingSet>       m_SetLayoutsVkDescriptorTypes;
            };

            constexpr static uint32_t BindlessDescriptorSetDesiredSampledImageCount = 1024;
            constexpr static uint32_t DescriptorSetReservedSampledImageCount = 32;

            void PickPhysicalDeviceAndCreate( InitConfig const& config );
			bool CheckAndCollectDeviceLayers( InitConfig const& config );
			bool CheckAndCollectDeviceExtensions( InitConfig const& config );
			bool CreateDevice( InitConfig const& config );

            // Pipeline State
            //-------------------------------------------------------------------------

            bool CreateRasterPipelineStateLayout( RHI::RHIRasterPipelineStateCreateDesc const& createDesc, CompiledShaderArray const& compiledShaders, VulkanPipelineState* pPipelineState );
            void DestroyRasterPipelineStateLayout( VulkanPipelineState* pPipelineState );
            CombinedShaderSetLayout CombinedAllShaderSetLayouts( CompiledShaderArray const& compiledShaders );
            TPair<VkDescriptorSetLayout, TMap<uint32_t, VkDescriptorType>> CreateDescriptorSetLayout( uint32_t set, CombinedShaderBindingLayout const& combinedSetBindingLayout, VkShaderStageFlags stage );

            // Static Resources
            //-------------------------------------------------------------------------
            
            void CreateStaticSamplers();
            VkSampler FindImmutableSampler( String const& indicateString );
            void DestroyStaticSamplers();

            // Utility Functions
            //-------------------------------------------------------------------------
		
            bool VulkanDevice::GetMemoryType( uint32_t typeBits, VkMemoryPropertyFlags properties, uint32_t& memTypeFound ) const;
            uint32_t GetMaxBindlessDescriptorSampledImageCount() const;

		private:

			TSharedPtr<VulkanInstance>			            m_pInstance = nullptr;
            TSharedPtr<VulkanSurface>                       m_pSurface = nullptr;

			VkDevice							            m_pHandle = nullptr;
			CollectedInfo						            m_collectInfos;
			VulkanPhysicalDevice				            m_physicalDevice;

			VulkanQueue							            m_globalGraphicQueue;
            VulkanMemoryAllocator                           m_globalMemoryAllcator;

            THashMap<VulkanStaticSamplerDesc, VkSampler>    m_immutableSamplers;
		};
	}
}

#endif