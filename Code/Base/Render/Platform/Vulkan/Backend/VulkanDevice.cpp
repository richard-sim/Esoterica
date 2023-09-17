#if defined(EE_VULKAN)
#include "VulkanDevice.h"
#include "VulkanCommonSettings.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanShader.h"
#include "VulkanSemaphore.h"
#include "VulkanUtils.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "RHIToVulkanSpecification.h"
#include "Base/Logging/Log.h"
#include "Base/Types/HashMap.h"
#include "Base/Resource/ResourcePtr.h"

namespace EE::Render
{
	namespace Backend
	{
		VulkanQueue::VulkanQueue( VulkanDevice const& device, QueueFamily const& queueFamily )
			: m_queueFamily( queueFamily )
		{
			vkGetDeviceQueue( device.m_pHandle, queueFamily.m_index, 0, &m_pHandle );

			if ( queueFamily.IsGraphicQueue() )
			{
				m_type = Type::Graphic;
			}
			else if ( queueFamily.IsComputeQueue() )
			{
				m_type = Type::Compute;
			}
			else if ( queueFamily.IsTransferQueue() )
			{
				m_type = Type::Transfer;
			}

			EE_ASSERT( m_pHandle != nullptr );
		}

		//-------------------------------------------------------------------------

		VulkanDevice::InitConfig VulkanDevice::InitConfig::GetDefault( bool enableDebug )
		{
			InitConfig config;
			config.m_requiredLayers = GetEngineVulkanDeviceRequiredLayers( enableDebug );
			config.m_requiredExtensions = GetEngineVulkanDeviceRequiredExtensions();
			return config;
		}

		//-------------------------------------------------------------------------

		VulkanDevice::VulkanDevice()
		{
            m_pInstance = MakeShared<VulkanInstance>();
            EE_ASSERT( m_pInstance != nullptr );

            m_pSurface = MakeShared<VulkanSurface>( m_pInstance );
            EE_ASSERT( m_pSurface != nullptr );

            InitConfig const config = InitConfig::GetDefault( m_pInstance->IsEnableDebug() );

            PickPhysicalDeviceAndCreate( config );

            m_globalMemoryAllcator.Initialize( this );

            CreateStaticSamplers();
        }

        VulkanDevice::VulkanDevice( InitConfig config )
		{
            m_pInstance = MakeShared<VulkanInstance>();
            EE_ASSERT( m_pInstance != nullptr );

            m_pSurface = MakeShared<VulkanSurface>( m_pInstance );
            EE_ASSERT( m_pSurface != nullptr );

            PickPhysicalDeviceAndCreate( config );

            m_globalMemoryAllcator.Initialize( this );

            CreateStaticSamplers();
		}

		VulkanDevice::~VulkanDevice()
		{
			EE_ASSERT( m_pHandle != nullptr );

            DestroyStaticSamplers();
            m_globalMemoryAllcator.Shutdown();

			vkDestroyDevice( m_pHandle, nullptr );
			m_pHandle = nullptr;
		}

		//-------------------------------------------------------------------------

        RHI::RHITexture* VulkanDevice::CreateTexture( RHI::RHITextureCreateDesc const& createDesc )
        {
            EE_ASSERT( createDesc.IsValid() );

            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.pNext = nullptr;
            imageCreateInfo.imageType = ToVulkanImageType( createDesc.m_type );
            imageCreateInfo.format = ToVulkanFormat( createDesc.m_format );
            imageCreateInfo.extent = { createDesc.m_width, createDesc.m_height, createDesc.m_depth };
            imageCreateInfo.mipLevels = createDesc.m_mipmap;
            imageCreateInfo.arrayLayers = createDesc.m_array;
            imageCreateInfo.samples = ToVulkanSampleCountFlags( createDesc.m_sample );
            imageCreateInfo.tiling = ToVulkanImageTiling( createDesc.m_tiling );
            imageCreateInfo.usage = ToVulkanImageUsageFlags( createDesc.m_usage );
            imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.flags = ToVulkanImageCreateFlags( createDesc.m_flag );
            // TODO: queue specified resource
            imageCreateInfo.pQueueFamilyIndices = nullptr;
            imageCreateInfo.queueFamilyIndexCount = 0;

            bool bRequireDedicatedMemory = false;
            uint32_t allcatedMemorySize = 0;

            #if VULKAN_USE_VMA_ALLOCATION
            VmaAllocationCreateInfo vmaAllocationCI = {};
            vmaAllocationCI.flags = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;
            if ( createDesc.m_usage.AreAnyFlagsSet( RHI::ETextureUsage::Color, RHI::ETextureUsage::DepthStencil )
                 && createDesc.m_width >= 1024
                 && createDesc.m_height >= 1024 )
            {
                vmaAllocationCI.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
                bRequireDedicatedMemory = true;
            }

            // TODO: For now, all texture should be GPU Only.
            vmaAllocationCI.usage = ToVmaMemoryUsage( createDesc.m_memoryUsage );

            VulkanTexture* pVkTexture = EE::New<VulkanTexture>();
            if ( pVkTexture )
            {
                VmaAllocationInfo allocInfo = {};
                VK_SUCCEEDED( vmaCreateImage( m_globalMemoryAllcator.m_pHandle, &imageCreateInfo, &vmaAllocationCI, &(pVkTexture->m_pHandle), &(pVkTexture->m_allocation), &allocInfo ) );
            
                allcatedMemorySize = static_cast<uint32_t>( allocInfo.size );
            }
            else
            {
                EE::Delete( pVkTexture );
            }
            #else
            VulkanTexture* pVkTexture = EE::New<VulkanTexture>();
            if ( pVkTexture )
            {
                VK_SUCCEEDED( vkCreateImage( m_pHandle, &imageCreateInfo, nullptr, &(pVkTexture->m_pHandle) ) );
            }
            else
            {
                EE::Delete( pVkTexture );
            }

            VkMemoryRequirements memReqs = {};
            vkGetImageMemoryRequirements( m_pHandle, pVkTexture->m_pHandle, &memReqs );

            allcatedMemorySize = static_cast<uint32_t>( memReqs.size );

            VkMemoryAllocateInfo memAllloc = {};
            memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memAllloc.allocationSize = memReqs.size;
            GetMemoryType( memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memAllloc.memoryTypeIndex );

            vkAllocateMemory( m_pHandle, &memAllloc, nullptr, &(pVkTexture->m_allocation) );
            vkBindImageMemory( m_pHandle, pVkTexture->m_pHandle, pVkTexture->m_allocation, 0);
            #endif // VULKAN_USE_VMA_ALLOCATION

            pVkTexture->m_desc = createDesc;
            // TODO: [WARNING] User's observation is different before render resource is created.
            // We modified or overwrite some property during creation, but user doesn't know it at all.
            // This is no a good design i think.
            if ( bRequireDedicatedMemory )
            {
                pVkTexture->m_desc.m_memoryFlag.SetFlag( RHI::ERenderResourceMemoryFlag::DedicatedMemory );
            }
            pVkTexture->m_desc.m_allocatedSize = allcatedMemorySize;

            return pVkTexture;
        }

        void VulkanDevice::DestroyTexture( RHI::RHITexture* pTexture )
        {
            EE_ASSERT( pTexture != nullptr );
            VulkanTexture* pVkTexture = static_cast<VulkanTexture*>( pTexture );
            EE_ASSERT( pVkTexture->m_pHandle != nullptr );
        
            #if VULKAN_USE_VMA_ALLOCATION
            if ( pVkTexture->m_allocation )
            {
                vmaDestroyImage( m_globalMemoryAllcator.m_pHandle, pVkTexture->m_pHandle, pVkTexture->m_allocation );
            }
            #else
            if ( pVkTexture->m_allocation != 0 )
            {
                vkDestroyImage( m_pHandle, pVkTexture->m_pHandle, nullptr);
                vkFreeMemory( m_pHandle, pVkTexture->m_allocation, nullptr );
            }
            #endif // VULKAN_USE_VMA_ALLOCATION

            EE::Delete( pTexture );
        }

        RHI::RHIBuffer* VulkanDevice::CreateBuffer( RHI::RHIBufferCreateDesc const& createDesc )
        {
            EE_ASSERT( createDesc.IsValid() );

            VkDeviceSize bufferSize = createDesc.m_desireSize;

            VkBufferCreateInfo bufferCreateInfo = {};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = bufferSize;
            bufferCreateInfo.usage = ToVulkanBufferUsageFlags( createDesc.m_usage );
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            uint32_t allcatedMemorySize = 0;

            #if VULKAN_USE_VMA_ALLOCATION
            VmaAllocationCreateInfo vmaAllocationCI = {};
            vmaAllocationCI.usage = ToVmaMemoryUsage( createDesc.m_memoryUsage );
            vmaAllocationCI.flags = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;
            if ( createDesc.m_memoryFlag.IsFlagSet(RHI::ERenderResourceMemoryFlag::DedicatedMemory) )
                vmaAllocationCI.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            if ( createDesc.m_memoryFlag.IsFlagSet( RHI::ERenderResourceMemoryFlag::PersistentMapping ) )
                vmaAllocationCI.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VulkanBuffer* pVkBuffer = EE::New<VulkanBuffer>();

            if ( pVkBuffer )
            {
                VmaAllocationInfo allocInfo = {};
                VK_SUCCEEDED( vmaCreateBuffer( m_globalMemoryAllcator.m_pHandle, &bufferCreateInfo, &vmaAllocationCI, &(pVkBuffer->m_pHandle), &(pVkBuffer->m_allocation), &allocInfo));
            
                allcatedMemorySize = static_cast<uint32_t>( allocInfo.size );
                if ( createDesc.m_memoryFlag.IsFlagSet( RHI::ERenderResourceMemoryFlag::PersistentMapping ) )
                {
                    pVkBuffer->m_pMappedMemory = allocInfo.pMappedData;
                }
            }
            else
            {
                EE::Delete( pVkBuffer );
            }
            #else
            VulkanBuffer* pVkBuffer = EE::New<VulkanBuffer>();

            if ( pVkBuffer )
            {
                VK_SUCCEEDED( vkCreateBuffer( m_pHandle, &bufferCreateInfo, nullptr, &(pVkBuffer->m_pHandle) ) );

            }
            else
            {
                EE::Delete( pVkBuffer );
            }

            VkMemoryRequirements memReqs;
            vkGetBufferMemoryRequirements( m_pHandle, pVkBuffer->m_pHandle, &memReqs );
            VkMemoryAllocateInfo memAlloc = {};
            memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memAlloc.allocationSize = memReqs.size;

            allcatedMemorySize = static_cast<uint32_t>( memReqs.size );

            if ( createDesc.m_memoryUsage == RHI::ERenderResourceMemoryUsage::GPUOnly )
            {
                GetMemoryType( memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memAlloc.memoryTypeIndex );
            }
            else if ( createDesc.m_memoryUsage == RHI::ERenderResourceMemoryUsage::CPUToGPU
                      || createDesc.m_memoryUsage == RHI::ERenderResourceMemoryUsage::CPUOnly )
            {
                GetMemoryType( memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memAlloc.memoryTypeIndex );
            }
            else
            {
                EE_UNREACHABLE_CODE();
            }

            VK_SUCCEEDED( vkAllocateMemory( m_pHandle, &memAlloc, nullptr, &(pVkBuffer->m_allocation) ));
            VK_SUCCEEDED( vkBindBufferMemory( m_pHandle, pVkBuffer->m_pHandle, pVkBuffer->m_allocation, 0));
            #endif // VULKAN_USE_VMA_ALLOCATION

            pVkBuffer->m_desc.m_allocatedSize = allcatedMemorySize;

            return pVkBuffer;
        }

        void VulkanDevice::DestroyBuffer( RHI::RHIBuffer* pBuffer )
        {
            EE_ASSERT( pBuffer != nullptr );
            VulkanBuffer* pVkBuffer = static_cast<VulkanBuffer*>( pBuffer );
            EE_ASSERT( pVkBuffer->m_pHandle != nullptr );

            #if VULKAN_USE_VMA_ALLOCATION
            if ( pVkBuffer->m_allocation )
            {
                vmaDestroyBuffer( m_globalMemoryAllcator.m_pHandle, pVkBuffer->m_pHandle, pVkBuffer->m_allocation );
            }
            #else
            if ( pVkBuffer->m_allocation != 0 )
            {
                vkDestroyBuffer( m_pHandle, pVkBuffer->m_pHandle, nullptr );
                vkFreeMemory( m_pHandle, pVkBuffer->m_allocation, nullptr );
            }
            #endif // VULKAN_USE_VMA_ALLOCATION

            EE::Delete( pVkBuffer );
        }

        RHI::RHISemaphore* VulkanDevice::CreateSyncSemaphore( RHI::RHISemaphoreCreateDesc const& createDesc )
        {
            EE_ASSERT( createDesc.IsValid() );

            VulkanSemaphore* pVkSemaphore = EE::New<VulkanSemaphore>();
            VkSemaphoreCreateInfo semaphoreCI = {};
            semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreCI.pNext = nullptr;
            semaphoreCI.flags = VkFlags( 0 );

            VK_SUCCEEDED( vkCreateSemaphore( m_pHandle, &semaphoreCI, nullptr, &(pVkSemaphore->m_pHandle) ) );

            return pVkSemaphore;
        }

        void VulkanDevice::DestroySyncSemaphore( RHI::RHISemaphore* pShader )
        {
            EE_ASSERT( pShader != nullptr );
            VulkanSemaphore* pVkSemaphore = static_cast<VulkanSemaphore*>( pShader );
            EE_ASSERT( pVkSemaphore->m_pHandle != nullptr );

            vkDestroySemaphore( m_pHandle, pVkSemaphore->m_pHandle, nullptr );
            
            EE::Delete( pVkSemaphore );
        }

        RHI::RHIShader* VulkanDevice::CreateShader( RHI::RHIShaderCreateDesc const& createDesc )
        {
            EE_ASSERT( createDesc.IsValid() );

            VulkanShader* pVkShader = EE::New<VulkanShader>();
            VkShaderModuleCreateInfo shaderModuleCI = {};
            shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleCI.pCode = reinterpret_cast<uint32_t const*>( createDesc.m_byteCode.data() );
            shaderModuleCI.codeSize = createDesc.m_byteCode.size(); // Note: here is the size in bytes!!

            VK_SUCCEEDED( vkCreateShaderModule( m_pHandle, &shaderModuleCI, nullptr, &(pVkShader->m_pModule) ) );

            return pVkShader;
        }

		void VulkanDevice::DestroyShader( RHI::RHIShader* pResource )
		{
            EE_ASSERT( pResource != nullptr );
            VulkanShader* pVkShader = static_cast<VulkanShader*>( pResource );
            EE_ASSERT( pVkShader->m_pModule != nullptr );

            vkDestroyShaderModule( m_pHandle, pVkShader->m_pModule, nullptr );

            EE::Delete( pVkShader );
		}

        //-------------------------------------------------------------------------

        RHI::RHIPipelineState* VulkanDevice::CreateRasterPipelineState( RHI::RHIRasterPipelineStateCreateDesc const& createDesc, CompiledShaderArray const& compiledShaders )
        {
            return nullptr;
        }

        void VulkanDevice::DestroyRasterPipelineState( RHI::RHIPipelineState* pPipelineState )
        {

        }

		//-------------------------------------------------------------------------

        void VulkanDevice::PickPhysicalDeviceAndCreate( InitConfig const& config )
        {
            auto pdDevices = m_pInstance->EnumeratePhysicalDevice();

            for ( auto& pd : pdDevices )
            {
                pd.CalculatePickScore( m_pSurface );
            }

            auto physicalDevice = PickMostSuitablePhysicalDevice( pdDevices );
            m_physicalDevice = std::move( physicalDevice );

            EE_ASSERT( CheckAndCollectDeviceLayers( config ) );
            EE_ASSERT( CheckAndCollectDeviceExtensions( config ) );

            EE_ASSERT( CreateDevice( config ) );
        }

		bool VulkanDevice::CheckAndCollectDeviceLayers( InitConfig const& config )
		{
			uint32_t layerCount = 0;
			VK_SUCCEEDED( vkEnumerateDeviceLayerProperties( m_physicalDevice.m_pHandle, &layerCount, nullptr ) );

			EE_ASSERT( layerCount > 0 );

			TVector<VkLayerProperties> layerProps( layerCount );
			VK_SUCCEEDED( vkEnumerateDeviceLayerProperties( m_physicalDevice.m_pHandle, &layerCount, layerProps.data() ) );
			m_collectInfos.m_deviceLayerProps = layerProps;

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
					EE_LOG_ERROR( "Render", "Vulkan Backend", "Device layer not found: %s", required );
					return false;
				}
			}

			return true;
		}

		bool VulkanDevice::CheckAndCollectDeviceExtensions( InitConfig const& config )
		{
			uint32_t extCount = 0;
			VK_SUCCEEDED( vkEnumerateDeviceExtensionProperties( m_physicalDevice.m_pHandle, nullptr, &extCount, nullptr ) );

			EE_ASSERT( extCount > 0 );

			TVector<VkExtensionProperties> extProps( extCount );
			VK_SUCCEEDED( vkEnumerateDeviceExtensionProperties( m_physicalDevice.m_pHandle, nullptr, &extCount, extProps.data() ) );
			m_collectInfos.m_deviceExtensionProps = extProps;

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
					EE_LOG_ERROR( "Render", "Vulkan Backend", "Device extension not found: %s", required );
					return false;
				}
			}

			return true;
		}

		bool VulkanDevice::CreateDevice( InitConfig const& config )
		{
			// device queue creation info population
			//-------------------------------------------------------------------------

			TVector<VkDeviceQueueCreateInfo> deviceQueueCIs = {};
			TVector<QueueFamily> deviceQueueFamilies = {};

			float priorities[] = { 1.0f };

			// only create one graphic queue for now
			for ( auto const& qf : m_physicalDevice.m_queueFamilies )
			{
				if ( qf.IsGraphicQueue() )
				{
					VkDeviceQueueCreateInfo dqCI = {};
					dqCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					dqCI.flags = VkFlags( 0 );
					dqCI.pNext = nullptr;

					dqCI.queueCount = 1;
					dqCI.queueFamilyIndex = qf.m_index;
					dqCI.pQueuePriorities = priorities;

					deviceQueueCIs.push_back( dqCI );
					deviceQueueFamilies.push_back( qf );
					break;
				}
			}

			if ( deviceQueueCIs.empty() )
			{
				EE_LOG_ERROR( "Render", "Vulkan Backend", "Invalid physical device which not supports graphic queue!" );
				return false;
			}

			// physical device features2 validation
			//-------------------------------------------------------------------------

			auto descriptor_indexing = VkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
			descriptor_indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

			auto imageless_framebuffer = VkPhysicalDeviceImagelessFramebufferFeaturesKHR{};
			imageless_framebuffer.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR;

			auto buffer_address = VkPhysicalDeviceBufferDeviceAddressFeaturesEXT{};
			buffer_address.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT;

			// TODO: pNext chain
			VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
			physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

			physicalDeviceFeatures2.pNext = &descriptor_indexing;
			descriptor_indexing.pNext = &imageless_framebuffer;
			imageless_framebuffer.pNext = &buffer_address;

			vkGetPhysicalDeviceFeatures2( m_physicalDevice.m_pHandle, &physicalDeviceFeatures2 );

			EE_ASSERT( imageless_framebuffer.imagelessFramebuffer );
			EE_ASSERT( descriptor_indexing.descriptorBindingPartiallyBound );
			EE_ASSERT( buffer_address.bufferDeviceAddress );

			// device creation
			//-------------------------------------------------------------------------

			VkDeviceCreateInfo deviceCI = {};
			deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCI.flags = VkFlags( 0 );
			deviceCI.pNext = &physicalDeviceFeatures2;

			deviceCI.pQueueCreateInfos = deviceQueueCIs.data();
			deviceCI.queueCreateInfoCount = static_cast<uint32_t>( deviceQueueCIs.size() );

			deviceCI.ppEnabledLayerNames = config.m_requiredLayers.data();
			deviceCI.enabledLayerCount = static_cast<uint32_t>( config.m_requiredLayers.size() );
			deviceCI.ppEnabledExtensionNames = config.m_requiredExtensions.data();
			deviceCI.enabledExtensionCount = static_cast<uint32_t>( config.m_requiredExtensions.size() );

			VK_SUCCEEDED( vkCreateDevice( m_physicalDevice.m_pHandle, &deviceCI, nullptr, &m_pHandle ) );

			// fetch global device queue
			//-------------------------------------------------------------------------

			m_globalGraphicQueue = VulkanQueue( *this, deviceQueueFamilies[0] );

			return true;
		}

        //-------------------------------------------------------------------------

        bool VulkanDevice::CreatePipelineStateLayout( RHI::RHIRasterPipelineStateCreateDesc const& createDesc, CompiledShaderArray const& compiledShaders, VulkanPipelineState* pPipelineState )
        {
            EE_ASSERT( pPipelineState );
            EE_ASSERT( !createDesc.m_pipelineShaders.empty() );

            CombinedShaderSetLayout combinedSetLayouts = CombinedAllShaderSetLayouts( compiledShaders );

            return true;
        }

        VulkanDevice::CombinedShaderSetLayout VulkanDevice::CombinedAllShaderSetLayouts( CompiledShaderArray const& compiledShaders )
        {
            CombinedShaderSetLayout combinedSetLayouts;

            for ( auto const& compiledShader : compiledShaders )
            {
                if ( compiledShader )
                {
                    auto const& resourceBindingSetLayout = compiledShader->GetResourceBindingSetLayout();

                    // initialize combinedSetLayouts 
                    for ( uint32_t i = 0; i < static_cast<uint32_t>( resourceBindingSetLayout.size() ); ++i )
                    {
                        if ( combinedSetLayouts.find( i ) == combinedSetLayouts.end() )
                        {
                            combinedSetLayouts.insert( i );
                        }
                    }

                    // for each binding in set
                    for ( uint32_t i = 0; i < static_cast<uint32_t>( resourceBindingSetLayout.size() ); ++i )
                    {
                        auto& setLayout = resourceBindingSetLayout[i];
                        auto& combinedBindingLayout = combinedSetLayouts.at( i );

                        for ( Render::Shader::ResourceBinding const& binding : setLayout )
                        {
                            auto combinedBinding = combinedBindingLayout.find( binding.m_slot );
                            if ( combinedBinding == combinedBindingLayout.end() )
                            {
                                combinedBindingLayout.insert_or_assign( binding.m_slot, binding );
                            }
                            else
                            {
                                // do check
                                Render::Shader::ResourceBinding const& existsBinding = combinedBinding->second;
                                EE_ASSERT( existsBinding.m_ID == binding.m_ID );
                                EE_ASSERT( existsBinding.m_slot == binding.m_slot );
                                EE_ASSERT( existsBinding.m_bindingCount == binding.m_bindingCount );
                                EE_ASSERT( existsBinding.m_bindingResourceType == binding.m_bindingResourceType );
                            }
                        }
                    }

                }
            }

            return combinedSetLayouts;
        }

        TPair<VkDescriptorSetLayout, TMap<uint32_t, VkDescriptorType>> VulkanDevice::CreateDescriptorSetLayout( uint32_t set, CombinedShaderBindingLayout const& combinedSetBindingLayout, VkShaderStageFlags stage )
        {
            TVector<VkDescriptorSetLayoutBinding> vkBindings;
            vkBindings.reserve( combinedSetBindingLayout.size() );

            // Enable partially binding.
            // Note for VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT: 
            // If a descriptor has no memory access by shader, it can be invalid descriptor.
            TVector<VkDescriptorBindingFlags> vkBindingFlags;
            vkBindingFlags.resize( combinedSetBindingLayout.size() );
            for ( VkDescriptorBindingFlags& flag : vkBindingFlags )
            {
                flag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
            }

            VkDescriptorSetLayoutCreateFlags vkSetLayoutCreateFlag = 0;

            for ( auto const& bindingLayout : combinedSetBindingLayout )
            {
                auto bindingResourceType = bindingLayout.second.m_bindingResourceType;
                VkDescriptorType vkDescriptorType = ToVulkanDescriptorType( bindingLayout.second.m_bindingResourceType );

                switch ( bindingResourceType )
                {
                    case EE::Render::Shader::ReflectedBindingResourceType::SampledImage:
                    {
                        uint32_t descriptorCount = static_cast<uint32_t>( bindingLayout.second.m_bindingCount.m_count );

                        if ( bindingLayout.second.m_bindingCount.m_type == Render::Shader::BindingCountType::Dynamic )
                        {
                            // Bindless descriptor can only be used at the end of this set.
                            EE_ASSERT( bindingLayout.first == static_cast<uint32_t>( combinedSetBindingLayout.size() - 1 ) );

                            // Enable all bindless descriptor flags.
                            // This binding can be updated as long as it is not dynamically used by any shader invocations.
                            // Note: dynamically used by any shader invocations means any shader invocation executes an instruction that performs any memory access using this descriptor.
                            vkBindingFlags[bindingLayout.first] |= VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT
                                | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT
                                | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

                            // Indicate that the descriptors inside this descriptor pool can be updated after binding.
                            vkSetLayoutCreateFlag |= VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

                            descriptorCount = GetMaxBindlessDescriptorSampledImageCount();
                        }

                        VkDescriptorSetLayoutBinding newBinding = {};
                        newBinding.binding = bindingLayout.first;
                        newBinding.descriptorCount = descriptorCount;
                        newBinding.descriptorType = vkDescriptorType;
                        newBinding.pImmutableSamplers = nullptr;
                        newBinding.stageFlags = stage;

                        vkBindings.push_back( newBinding );

                        break;
                    }
                    case EE::Render::Shader::ReflectedBindingResourceType::StorageTexelBuffer:
                    case EE::Render::Shader::ReflectedBindingResourceType::StorageImage:
                    case EE::Render::Shader::ReflectedBindingResourceType::UniformTexelBuffer:
                    case EE::Render::Shader::ReflectedBindingResourceType::UniformBuffer:
                    case EE::Render::Shader::ReflectedBindingResourceType::StorageBuffer:
                    {
                        if ( bindingLayout.second.m_bindingCount.m_type == Render::Shader::BindingCountType::Dynamic )
                        {
                            EE_LOG_ERROR( "Render", "Vulkan Device", "StorageImage/UniformTexelBuffer/UniformBuffer/StorageBuffer doesn't support bindless descriptor set!");
                            EE_ASSERT( false );
                        }

                        VkDescriptorSetLayoutBinding newBinding = {};
                        newBinding.binding = bindingLayout.first;
                        newBinding.descriptorCount = static_cast<uint32_t>( bindingLayout.second.m_bindingCount.m_count );
                        newBinding.descriptorType = vkDescriptorType;
                        newBinding.pImmutableSamplers = nullptr;
                        newBinding.stageFlags = stage;

                        vkBindings.push_back( newBinding );

                        break;
                    }
                    case EE::Render::Shader::ReflectedBindingResourceType::Sampler:
                    {
                        EE_UNIMPLEMENTED_FUNCTION();
                        break;
                    }
                    case EE::Render::Shader::ReflectedBindingResourceType::CombinedImageSampler:
                    case EE::Render::Shader::ReflectedBindingResourceType::InputAttachment:
                    {
                        EE_UNIMPLEMENTED_FUNCTION();
                        break;
                    }
                    default:
                    EE_UNREACHABLE_CODE();
                    break;
                }
            }

            EE_UNREACHABLE_CODE();
            return {};
        }

        // Static Resources
        //-------------------------------------------------------------------------

        void VulkanDevice::CreateStaticSamplers()
        {
            #define SIZE_OF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))

            VkFilter const allFilters[] = { VK_FILTER_LINEAR, VK_FILTER_NEAREST };
            VkSamplerMipmapMode const allMipmapModes[] = { VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST };
            VkSamplerAddressMode const allAddressModes[] = {
                VK_SAMPLER_ADDRESS_MODE_REPEAT,
                VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
            };

            for ( uint32_t filter = 0; filter < SIZE_OF_ARRAY( allFilters ); ++filter )
            {
                for ( uint32_t mipmap = 0; mipmap < SIZE_OF_ARRAY( allMipmapModes ); ++mipmap )
                {
                    for ( uint32_t address = 0; address < SIZE_OF_ARRAY( allAddressModes ); ++address )
                    {
                        VkSamplerCreateInfo samplerCreateInfo = {};
                        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                        samplerCreateInfo.minFilter = allFilters[filter];
                        samplerCreateInfo.magFilter = allFilters[filter];
                        samplerCreateInfo.mipmapMode = allMipmapModes[mipmap];
                        samplerCreateInfo.addressModeU = allAddressModes[address];
                        samplerCreateInfo.addressModeV = allAddressModes[address];
                        samplerCreateInfo.addressModeW = allAddressModes[address];
                        samplerCreateInfo.maxLod = VK_LOD_CLAMP_NONE;
                        samplerCreateInfo.maxAnisotropy = 16.0f;
                        samplerCreateInfo.anisotropyEnable = allFilters[filter] == VK_FILTER_LINEAR;

                        VulkanStaticSamplerDesc desc{ allFilters[filter], allMipmapModes[mipmap], allAddressModes[address] };
                        //VulkanStaticSamplerDesc desc{ .filter = allFilters[filter], .mipmap = allMipmapModes[mipmap], .address = allAddressModes[address] };

                        VkSampler newSampler = nullptr;
                        VK_SUCCEEDED( vkCreateSampler( m_pHandle, &samplerCreateInfo, nullptr, &newSampler ) );

                        m_immutableSamplers[desc] = newSampler;
                    }
                }
            }

            #undef SIZE_OF_ARRAY
        }

        void VulkanDevice::DestroyStaticSamplers()
        {
            for ( auto& sampler : m_immutableSamplers )
            {
                EE_ASSERT( sampler.second != nullptr );
                vkDestroySampler( m_pHandle, sampler.second, nullptr );
            }

            m_immutableSamplers.clear();
        }

        // Utility functions
        //-------------------------------------------------------------------------

        bool VulkanDevice::GetMemoryType( uint32_t typeBits, VkMemoryPropertyFlags properties, uint32_t& OutProperties ) const
        {
            for ( uint32_t i = 0; i < m_physicalDevice.m_memoryProps.memoryTypeCount; i++ )
            {
                if ( ( typeBits & 1 ) == 1 )
                {
                    if ( ( m_physicalDevice.m_memoryProps.memoryTypes[i].propertyFlags & properties ) == properties )
                    {
                        OutProperties = i;
                        return true;
                    }
                }
                typeBits >>= 1;
            }

            OutProperties = 0;
            return false;
        }

        uint32_t VulkanDevice::GetMaxBindlessDescriptorSampledImageCount() const
        {
            return Math::Min(
                m_physicalDevice.m_props.limits.maxPerStageDescriptorSampledImages - DescriptorSetReservedSampledImageCount,
                BindlessDescriptorSetDesiredSampledImageCount
                );
        }
    }
}

#endif