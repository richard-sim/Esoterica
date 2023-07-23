#if defined(EE_VULKAN)
#include "VulkanBarrier.h"

#include "Base/Types/Arrays.h"

namespace EE::Render
{
	namespace Backend
	{
		namespace
		{
			static TInlineVector<VkMemoryBarrier, 1> gGlobalBarriers;
			static TInlineVector<VkBufferMemoryBarrier, 32> gBufferBarriers;
			static TInlineVector<VkImageMemoryBarrier, 32> gImageBarriers;
		}

		struct AccessInfo
		{
			/// Describes which stage in the pipeline this resource is used.
			VkPipelineStageFlags			m_stageMask;
			/// Describes which access mode in the pipeline this resource is used.
			VkAccessFlags					m_accessMask;
			/// Describes the image memory layout which image will be used if this resource is a image resource.
			VkImageLayout					m_imageLayout;
		};

		inline static VkImageAspectFlags ToVkImageAspectFlags( TBitFlags<ImageAspectFlags> const& flags )
		{
			VkImageAspectFlags flag = 0;
			if ( flags.IsFlagSet( ImageAspectFlags::Color ) )
			{
				flag |= VK_IMAGE_ASPECT_COLOR_BIT;
			}
			if ( flags.IsFlagSet( ImageAspectFlags::Depth ) )
			{
				flag |= VK_IMAGE_ASPECT_DEPTH_BIT;
			}
			if ( flags.IsFlagSet( ImageAspectFlags::Stencil ) )
			{
				flag |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			if ( flags.IsFlagSet( ImageAspectFlags::Metadata ) )
			{
				flag |= VK_IMAGE_ASPECT_METADATA_BIT;
			}
			return flag;
		}

		static bool IsWriteAccess( RenderResourceBarrierState const& access )
		{
			switch ( access )
			{
				case RenderResourceBarrierState::VertexShaderWrite:
				case RenderResourceBarrierState::TessellationControlShaderWrite:
				case RenderResourceBarrierState::TessellationEvaluationShaderWrite:
				case RenderResourceBarrierState::GeometryShaderWrite:
				case RenderResourceBarrierState::FragmentShaderWrite:
				case RenderResourceBarrierState::ColorAttachmentWrite:
				case RenderResourceBarrierState::DepthStencilAttachmentWrite:
				case RenderResourceBarrierState::DepthAttachmentWriteStencilReadOnly:
				case RenderResourceBarrierState::StencilAttachmentWriteDepthReadOnly:
				case RenderResourceBarrierState::ComputeShaderWrite:
				case RenderResourceBarrierState::AnyShaderWrite:
				case RenderResourceBarrierState::TransferWrite:
				case RenderResourceBarrierState::HostWrite:

				case RenderResourceBarrierState::General:
				case RenderResourceBarrierState::ColorAttachmentReadWrite:

				return true;
				break;

				default:
				return false;
				break;
			}
		}

		static AccessInfo GetAccessInfo( RenderResourceBarrierState const& barrierState )
		{
			switch ( barrierState )
			{
				case RenderResourceBarrierState::Undefined:
				return AccessInfo{ 0, 0, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::IndirectBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::VertexBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::IndexBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_INDEX_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };

				case RenderResourceBarrierState::VertexShaderReadUniformBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::VertexShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::VertexShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::TessellationControlShaderReadUniformBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::TessellationControlShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::TessellationControlShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::TessellationEvaluationShaderReadUniformBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::TessellationEvaluationShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::TessellationEvaluationShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::GeometryShaderReadUniformBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::GeometryShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::GeometryShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::FragmentShaderReadUniformBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::FragmentShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				// QA
				case RenderResourceBarrierState::FragmentShaderReadColorInputAttachment:
				return AccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				// QA
				case RenderResourceBarrierState::FragmentShaderReadDepthStencilInputAttachment:
				return AccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::FragmentShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::ColorAttachmentRead:
				return AccessInfo{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
				case RenderResourceBarrierState::DepthStencilAttachmentRead:
				return AccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };

				case RenderResourceBarrierState::ComputeShaderReadUniformBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::ComputeShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::ComputeShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::AnyShaderReadUniformBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::AnyShaderReadUniformBufferOrVertexBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::AnyShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::AnyShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::TransferRead:
				return AccessInfo{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL };
				case RenderResourceBarrierState::HostRead:
				return AccessInfo{ VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_HOST_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };
				case RenderResourceBarrierState::Present:
				return AccessInfo{ 0, 0, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };

				case RenderResourceBarrierState::VertexShaderWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
				case RenderResourceBarrierState::TessellationControlShaderWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
				case RenderResourceBarrierState::TessellationEvaluationShaderWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
				case RenderResourceBarrierState::GeometryShaderWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
				case RenderResourceBarrierState::FragmentShaderWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::ColorAttachmentWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
				case RenderResourceBarrierState::DepthStencilAttachmentWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
				case RenderResourceBarrierState::DepthAttachmentWriteStencilReadOnly:
				return AccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::StencilAttachmentWriteDepthReadOnly:
				return AccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL };

				case RenderResourceBarrierState::ComputeShaderWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::AnyShaderWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
				case RenderResourceBarrierState::TransferWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL };
				case RenderResourceBarrierState::HostWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_HOST_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::ColorAttachmentReadWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
				case RenderResourceBarrierState::General:
				return AccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::RayTracingShaderReadSampledImageOrUniformTexelBuffer:
				return AccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::RayTracingShaderReadColorInputAttachment:
				return AccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::RayTracingShaderReadDepthStencilInputAttachment:
				return AccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };
				case RenderResourceBarrierState::RayTracingShaderReadAccelerationStructure:
				return AccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::RayTracingShaderReadOther:
				return AccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

				case RenderResourceBarrierState::AccelerationStructureBuildWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::AccelerationStructureBuildRead:
				return AccessInfo{ VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, VK_IMAGE_LAYOUT_UNDEFINED };
				case RenderResourceBarrierState::AccelerationStructureBufferWrite:
				return AccessInfo{ VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED };

				default:
				EE_UNREACHABLE_CODE();
				return AccessInfo{};
			}
		}

		//-------------------------------------------------------------------------

		void VulkanCmdPipelineBarrier(
			VulkanCommandBuffer* pCmdBuf,
			GlobalBarrier const* pGlobalBarriers,
			uint32_t bufferBarrierCount, BufferBarrier const* pBufferBarriers,
			uint32_t imageBarrierCount, ImageBarrier const* pImageBarriers )
		{
			VkPipelineStageFlags srcStageFlag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags dstStageFlag = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			
			gGlobalBarriers.reserve( 1 );
			gBufferBarriers.reserve( bufferBarrierCount );
			gImageBarriers.reserve( imageBarrierCount );

			if ( pGlobalBarriers != nullptr )
			{
				auto transition = GetMemoryBarrierTransition( *pGlobalBarriers );
				srcStageFlag |= transition.m_srcStage;
				dstStageFlag |= transition.m_dstStage;
				gGlobalBarriers.push_back( transition.m_barrier );
			}

			for ( uint32_t i = 0; i < bufferBarrierCount; ++i )
			{
				auto const& bufferBarrier = pBufferBarriers[i];
				auto transition = GetBufferBarrierTransition( bufferBarrier );
				srcStageFlag |= transition.m_srcStage;
				dstStageFlag |= transition.m_dstStage;
				gBufferBarriers.push_back( transition.m_barrier );
			}

			for ( uint32_t i = 0; i < imageBarrierCount; ++i )
			{
				auto const& imageBarrier = pImageBarriers[i];
				auto transition = GetImageBarrierTransition( imageBarrier );
				srcStageFlag |= transition.m_srcStage;
				dstStageFlag |= transition.m_dstStage;
				gImageBarriers.push_back( transition.m_barrier );
			}

			vkCmdPipelineBarrier(
				pCmdBuf->Raw(),
				srcStageFlag, dstStageFlag,
				0,
				static_cast<uint32_t>( gGlobalBarriers.size() ), gGlobalBarriers.data(),
				static_cast<uint32_t>( gBufferBarriers.size() ), gBufferBarriers.data(),
				static_cast<uint32_t>( gImageBarriers.size() ), gImageBarriers.data()
			);

			gGlobalBarriers.clear();
			gBufferBarriers.clear();
			gImageBarriers.clear();
		}

		//-------------------------------------------------------------------------

		MemoryBarrierTransition GetMemoryBarrierTransition( GlobalBarrier const& globalBarrier )
		{
			MemoryBarrierTransition barrier = {};
			barrier.m_srcStage = VkFlags( 0 );
			barrier.m_dstStage = VkFlags( 0 );
			barrier.m_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			barrier.m_barrier.pNext = nullptr;
			barrier.m_barrier.srcAccessMask = VkFlags( 0 );
			barrier.m_barrier.dstAccessMask = VkFlags( 0 );

			for ( uint32_t i = 0; i < globalBarrier.m_previousAccessesCount; ++i )
			{
				auto const& prevAccess = globalBarrier.m_pPreviousAccesses[i];
				auto accessInfo = GetAccessInfo( prevAccess );

				// what stage this resource is used in previous stage
				barrier.m_srcStage |= accessInfo.m_stageMask;

				// only access the write access
				if ( IsWriteAccess( prevAccess ) )
				{
					barrier.m_barrier.srcAccessMask |= accessInfo.m_accessMask;
				}
			}

			for ( uint32_t i = 0; i < globalBarrier.m_nextAccessesCount; ++i )
			{
				auto const& nextAccess = globalBarrier.m_pNextAccesses[i];
				auto accessInfo = GetAccessInfo( nextAccess );

				// what stage this resource is used in previous stage
				barrier.m_dstStage |= accessInfo.m_stageMask;

				// if write access happend before, it must be visible to the dst access.
				// (i.e. RAW (Read-After-Write) operation or WAW)
				if ( barrier.m_barrier.srcAccessMask != VkFlags( 0 ) )
				{
					barrier.m_barrier.dstAccessMask |= accessInfo.m_accessMask;
				}
			}

			// ensure that the stage masks are valid if no stages were determined
			if ( barrier.m_srcStage == VkFlags( 0 ) )
			{
				barrier.m_srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			}

			if ( barrier.m_dstStage == VkFlags( 0 ) )
			{
				barrier.m_dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			}

			return barrier;
		}

		BufferBarrierTransition GetBufferBarrierTransition( BufferBarrier const& bufferBarrier )
		{
			BufferBarrierTransition barrier = {};
			barrier.m_srcStage = VkFlags( 0 );
			barrier.m_dstStage = VkFlags( 0 );
			barrier.m_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			barrier.m_barrier.pNext = nullptr;
			barrier.m_barrier.srcAccessMask = VkFlags( 0 );
			barrier.m_barrier.dstAccessMask = VkFlags( 0 );
			barrier.m_barrier.srcQueueFamilyIndex = bufferBarrier.m_srcQueueFamilyIndex;
			barrier.m_barrier.dstQueueFamilyIndex = bufferBarrier.m_dstQueueFamilyIndex;
			barrier.m_barrier.buffer = reinterpret_cast<VkBuffer>( bufferBarrier.m_bufHandle.m_pData );
			barrier.m_barrier.offset = static_cast<VkDeviceSize>( bufferBarrier.m_offset );
			barrier.m_barrier.size = static_cast<VkDeviceSize>( bufferBarrier.m_size );

			for ( uint32_t i = 0; i < bufferBarrier.m_previousAccessesCount; ++i )
			{
				auto const& prevAccess = bufferBarrier.m_pPreviousAccesses[i];
				auto accessInfo = GetAccessInfo( prevAccess );

				// what stage this resource is used in previous stage
				barrier.m_srcStage |= accessInfo.m_stageMask;

				// only access the write access
				if ( IsWriteAccess( prevAccess ) )
				{
					barrier.m_barrier.srcAccessMask |= accessInfo.m_accessMask;
				}
			}

			for ( uint32_t i = 0; i < bufferBarrier.m_nextAccessesCount; ++i )
			{
				auto const& nextAccess = bufferBarrier.m_pNextAccesses[i];
				auto accessInfo = GetAccessInfo( nextAccess );

				// what stage this resource is used in previous stage
				barrier.m_dstStage |= accessInfo.m_stageMask;

				// if write access happend before, it must be visible to the dst access.
				// (i.e. RAW (Read-After-Write) operation or WAW)
				if ( barrier.m_barrier.srcAccessMask != VkFlags( 0 ) )
				{
					barrier.m_barrier.dstAccessMask |= accessInfo.m_accessMask;
				}
			}

			// ensure that the stage masks are valid if no stages were determined
			if ( barrier.m_srcStage == VkFlags( 0 ) )
			{
				barrier.m_srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			}

			if ( barrier.m_dstStage == VkFlags( 0 ) )
			{
				barrier.m_dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			}

			return barrier;
		}

		ImageBarrierTransition GetImageBarrierTransition( ImageBarrier const& imageBarrier )
		{
			ImageBarrierTransition barrier = {};
			barrier.m_srcStage = VkFlags( 0 );
			barrier.m_dstStage = VkFlags( 0 );
			barrier.m_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.m_barrier.pNext = nullptr;
			barrier.m_barrier.srcAccessMask = VkFlags( 0 );
			barrier.m_barrier.dstAccessMask = VkFlags( 0 );
			barrier.m_barrier.srcQueueFamilyIndex = imageBarrier.m_srcQueueFamilyIndex;
			barrier.m_barrier.dstQueueFamilyIndex = imageBarrier.m_dstQueueFamilyIndex;
			barrier.m_barrier.image = reinterpret_cast<VkImage>( imageBarrier.m_texHandle.m_pData );

			barrier.m_barrier.subresourceRange.aspectMask = ToVkImageAspectFlags( imageBarrier.m_subresourceRange.m_aspectFlags );
			barrier.m_barrier.subresourceRange.baseMipLevel = imageBarrier.m_subresourceRange.m_baseMipLevel;
			barrier.m_barrier.subresourceRange.levelCount = imageBarrier.m_subresourceRange.m_levelCount;
			barrier.m_barrier.subresourceRange.baseArrayLayer = imageBarrier.m_subresourceRange.m_baseArrayLayer;
			barrier.m_barrier.subresourceRange.layerCount = imageBarrier.m_subresourceRange.m_layerCount;

			for ( uint32_t i = 0; i < imageBarrier.m_previousAccessesCount; ++i )
			{
				auto const& prevAccess = imageBarrier.m_pPreviousAccesses[i];
				auto accessInfo = GetAccessInfo( prevAccess );

				// what stage this resource is used in previous stage
				barrier.m_srcStage |= accessInfo.m_stageMask;

				// only access the write access
				if ( IsWriteAccess( prevAccess ) )
				{
					barrier.m_barrier.srcAccessMask |= accessInfo.m_accessMask;
				}

				if ( imageBarrier.m_discardContents )
				{
					// we don't care about the previous image layout
					barrier.m_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				}
				else
				{
					switch ( imageBarrier.m_previousLayout )
					{
						case ImageMemoryLayout::Optimal:
						barrier.m_barrier.newLayout = accessInfo.m_imageLayout;
						break;
						case ImageMemoryLayout::General:
						{
							if ( prevAccess == RenderResourceBarrierState::Present )
							{
								barrier.m_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
							}
							else
							{
								barrier.m_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
							}
						}
						break;
						case ImageMemoryLayout::GeneralAndPresentation:
						EE_UNIMPLEMENTED_FUNCTION();
					}
				}
			}

			for ( uint32_t i = 0; i < imageBarrier.m_nextAccessesCount; ++i )
			{
				auto const& nextAccess = imageBarrier.m_pNextAccesses[i];
				auto accessInfo = GetAccessInfo( nextAccess );

				// what stage this resource is used in previous stage
				barrier.m_dstStage |= accessInfo.m_stageMask;

				// if write access happend before, it must be visible to the dst access.
				// (i.e. RAW (Read-After-Write) operation or WAW)
				if ( barrier.m_barrier.srcAccessMask != VkFlags( 0 ) )
				{
					barrier.m_barrier.dstAccessMask |= accessInfo.m_accessMask;
				}

				switch ( imageBarrier.m_nextLayout )
				{
					case ImageMemoryLayout::Optimal:
					barrier.m_barrier.oldLayout = accessInfo.m_imageLayout;
					break;
					case ImageMemoryLayout::General:
					{
						if ( nextAccess == RenderResourceBarrierState::Present )
						{
							barrier.m_barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
						}
						else
						{
							barrier.m_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
						}
					}
					break;
					case ImageMemoryLayout::GeneralAndPresentation:
					EE_UNIMPLEMENTED_FUNCTION();
				}
			}

			// ensure that the stage masks are valid if no stages were determined
			if ( barrier.m_srcStage == VkFlags( 0 ) )
			{
				barrier.m_srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			}

			if ( barrier.m_dstStage == VkFlags( 0 ) )
			{
				barrier.m_dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			}

			return barrier;
		}
	}
}

#endif // EE_VULKAN