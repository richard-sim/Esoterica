#if defined(EE_VULKAN)
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanPipelineState.h"

#include "Base/RHI/RHIDowncastHelper.h"

namespace EE::Render
{
    namespace Backend
    {
        TInlineVector<VkMemoryBarrier, 1> VulkanCommandBuffer::m_sGlobalBarriers;
        TInlineVector<VkBufferMemoryBarrier, 32> VulkanCommandBuffer::m_sBufferBarriers;
        TInlineVector<VkImageMemoryBarrier, 32> VulkanCommandBuffer::m_sTextureBarriers;

        // Render Commands
        //-------------------------------------------------------------------------

        void VulkanCommandBuffer::Draw( uint32_t vertexCount, uint32_t instanceCount /*= 1*/, uint32_t firstIndex /*= 0*/, uint32_t firstInstance /*= 0 */ )
        {
            vkCmdDraw( m_pHandle, vertexCount, instanceCount, firstIndex, firstInstance );
        }

        void VulkanCommandBuffer::DrawIndexed( uint32_t indexCount, uint32_t instanceCount /*= 1*/, uint32_t firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, uint32_t firstInstance /*= 0*/ )
        {
            vkCmdDrawIndexed( m_pHandle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
        }

        // Pipeline Barrier
        //-------------------------------------------------------------------------

        bool VulkanCommandBuffer::BeginRenderPass( RHI::RHIRenderPass* pRhiRenderPass, RHI::RenderArea const& renderArea )
        {
            if ( pRhiRenderPass )
            {
                if ( auto* pVkRenderPass = RHI::RHIDowncast<VulkanRenderPass>( pRhiRenderPass ) )
                {
                    VkRenderPassAttachmentBeginInfo attachmentBeginInfo = {};
                    attachmentBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO;
                    //attachmentBeginInfo.

                    VkRenderPassBeginInfo beginInfo = {};
                    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                    beginInfo.pNext = &attachmentBeginInfo;

                    vkCmdBeginRenderPass( m_pHandle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE );

                    return true;
                }
            }

            EE_LOG_WARNING( "Render", "VulkanCommandBuffer", "Pass in null render pass, reject to begin render pass!" );
            return false;
        }

        void VulkanCommandBuffer::EndRenderPass()
        {
            vkCmdEndRenderPass( m_pHandle );
        }

        void VulkanCommandBuffer::PipelineBarrier(
            RHI::GlobalBarrier const* pGlobalBarriers,
            uint32_t bufferBarrierCount, RHI::BufferBarrier const* pBufferBarriers,
            uint32_t textureBarrierCount, RHI::TextureBarrier const* pTextureBarriers 
        )
        {
            VkPipelineStageFlags srcStageFlag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags dstStageFlag = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

            if ( pGlobalBarriers != nullptr )
            {
                auto transition = GetMemoryBarrierTransition( *pGlobalBarriers );
                srcStageFlag |= transition.m_srcStage;
                dstStageFlag |= transition.m_dstStage;
                m_sGlobalBarriers.push_back( transition.m_barrier );
            }

            for ( uint32_t i = 0; i < bufferBarrierCount; ++i )
            {
                auto const& bufferBarrier = pBufferBarriers[i];
                auto transition = GetBufferBarrierTransition( bufferBarrier );
                srcStageFlag |= transition.m_srcStage;
                dstStageFlag |= transition.m_dstStage;
                m_sBufferBarriers.push_back( transition.m_barrier );
            }

            for ( uint32_t i = 0; i < textureBarrierCount; ++i )
            {
                auto const& textureBarrier = pTextureBarriers[i];
                auto transition = GetTextureBarrierTransition( textureBarrier );
                srcStageFlag |= transition.m_srcStage;
                dstStageFlag |= transition.m_dstStage;
                m_sTextureBarriers.push_back( transition.m_barrier );
            }

            vkCmdPipelineBarrier(
                m_pHandle,
                srcStageFlag, dstStageFlag,
                0,
                static_cast<uint32_t>( m_sGlobalBarriers.size() ), m_sGlobalBarriers.data(),
                static_cast<uint32_t>( m_sBufferBarriers.size() ), m_sBufferBarriers.data(),
                static_cast<uint32_t>( m_sTextureBarriers.size() ), m_sTextureBarriers.data()
            );

            m_sGlobalBarriers.clear();
            m_sBufferBarriers.clear();
            m_sTextureBarriers.clear();
        }
    
        // Resource Binding
        //-------------------------------------------------------------------------

        void VulkanCommandBuffer::BindPipelineState( RHI::RHIPipelineState* pRhiPipelineState )
        {
            if ( pRhiPipelineState )
            {
                if ( auto* pVkPipelineState = RHI::RHIDowncast<VulkanPipelineState>( pRhiPipelineState ) )
                {
                    vkCmdBindPipeline( m_pHandle, pVkPipelineState->m_pipelineBindPoint, pVkPipelineState->m_pPipeline );
                }
            }
            else
            {
                EE_LOG_WARNING( "Render", "VulkanCommandBuffer", "Pass in null pipeline state, reject to bind pipeline state!" );
            }
        }

        // Vulkan Pipeline Barrier Utility Functions
        //-------------------------------------------------------------------------
    
        inline static VkImageAspectFlags ToVkImageAspectFlags( TBitFlags<RHI::TextureAspectFlags> const& flags )
        {
            VkImageAspectFlags flag = 0;
            if ( flags.IsFlagSet( RHI::TextureAspectFlags::Color ) )
            {
                flag |= VK_IMAGE_ASPECT_COLOR_BIT;
            }
            if ( flags.IsFlagSet( RHI::TextureAspectFlags::Depth ) )
            {
                flag |= VK_IMAGE_ASPECT_DEPTH_BIT;
            }
            if ( flags.IsFlagSet( RHI::TextureAspectFlags::Stencil ) )
            {
                flag |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
            if ( flags.IsFlagSet( RHI::TextureAspectFlags::Metadata ) )
            {
                flag |= VK_IMAGE_ASPECT_METADATA_BIT;
            }
            return flag;
        }

        static bool IsWriteAccess( RHI::RenderResourceBarrierState const& access )
        {
            switch ( access )
            {
                case RHI::RenderResourceBarrierState::VertexShaderWrite:
                case RHI::RenderResourceBarrierState::TessellationControlShaderWrite:
                case RHI::RenderResourceBarrierState::TessellationEvaluationShaderWrite:
                case RHI::RenderResourceBarrierState::GeometryShaderWrite:
                case RHI::RenderResourceBarrierState::FragmentShaderWrite:
                case RHI::RenderResourceBarrierState::ColorAttachmentWrite:
                case RHI::RenderResourceBarrierState::DepthStencilAttachmentWrite:
                case RHI::RenderResourceBarrierState::DepthAttachmentWriteStencilReadOnly:
                case RHI::RenderResourceBarrierState::StencilAttachmentWriteDepthReadOnly:
                case RHI::RenderResourceBarrierState::ComputeShaderWrite:
                case RHI::RenderResourceBarrierState::AnyShaderWrite:
                case RHI::RenderResourceBarrierState::TransferWrite:
                case RHI::RenderResourceBarrierState::HostWrite:

                case RHI::RenderResourceBarrierState::General:
                case RHI::RenderResourceBarrierState::ColorAttachmentReadWrite:

                return true;
                break;

                default:
                return false;
                break;
            }
        }

        static VkAccessInfo GetAccessInfo( RHI::RenderResourceBarrierState const& barrierState )
        {
            switch ( barrierState )
            {
                case RHI::RenderResourceBarrierState::Undefined:
                return VkAccessInfo{ 0, 0, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::IndirectBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::VertexBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::IndexBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_INDEX_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };

                case RHI::RenderResourceBarrierState::VertexShaderReadUniformBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::VertexShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::VertexShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::TessellationControlShaderReadUniformBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::TessellationControlShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::TessellationControlShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::TessellationEvaluationShaderReadUniformBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::TessellationEvaluationShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::TessellationEvaluationShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::GeometryShaderReadUniformBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::GeometryShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::GeometryShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::FragmentShaderReadUniformBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::FragmentShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                // QA
                case RHI::RenderResourceBarrierState::FragmentShaderReadColorInputAttachment:
                return VkAccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                // QA
                case RHI::RenderResourceBarrierState::FragmentShaderReadDepthStencilInputAttachment:
                return VkAccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::FragmentShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::ColorAttachmentRead:
                return VkAccessInfo{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
                case RHI::RenderResourceBarrierState::DepthStencilAttachmentRead:
                return VkAccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };

                case RHI::RenderResourceBarrierState::ComputeShaderReadUniformBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::ComputeShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::ComputeShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::AnyShaderReadUniformBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::AnyShaderReadUniformBufferOrVertexBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::AnyShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::AnyShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::TransferRead:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL };
                case RHI::RenderResourceBarrierState::HostRead:
                return VkAccessInfo{ VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_HOST_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };
                case RHI::RenderResourceBarrierState::Present:
                return VkAccessInfo{ 0, 0, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };

                case RHI::RenderResourceBarrierState::VertexShaderWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
                case RHI::RenderResourceBarrierState::TessellationControlShaderWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
                case RHI::RenderResourceBarrierState::TessellationEvaluationShaderWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
                case RHI::RenderResourceBarrierState::GeometryShaderWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
                case RHI::RenderResourceBarrierState::FragmentShaderWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::ColorAttachmentWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
                case RHI::RenderResourceBarrierState::DepthStencilAttachmentWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                case RHI::RenderResourceBarrierState::DepthAttachmentWriteStencilReadOnly:
                return VkAccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::StencilAttachmentWriteDepthReadOnly:
                return VkAccessInfo{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL };

                case RHI::RenderResourceBarrierState::ComputeShaderWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::AnyShaderWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };
                case RHI::RenderResourceBarrierState::TransferWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL };
                case RHI::RenderResourceBarrierState::HostWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_HOST_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::ColorAttachmentReadWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
                case RHI::RenderResourceBarrierState::General:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::RayTracingShaderReadSampledImageOrUniformTexelBuffer:
                return VkAccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::RayTracingShaderReadColorInputAttachment:
                return VkAccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::RayTracingShaderReadDepthStencilInputAttachment:
                return VkAccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };
                case RHI::RenderResourceBarrierState::RayTracingShaderReadAccelerationStructure:
                return VkAccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::RayTracingShaderReadOther:
                return VkAccessInfo{ VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL };

                case RHI::RenderResourceBarrierState::AccelerationStructureBuildWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::AccelerationStructureBuildRead:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, VK_IMAGE_LAYOUT_UNDEFINED };
                case RHI::RenderResourceBarrierState::AccelerationStructureBufferWrite:
                return VkAccessInfo{ VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED };

                default:
                EE_UNREACHABLE_CODE();
                return VkAccessInfo{};
            }
        }

        VkMemoryBarrierTransition VulkanCommandBuffer::GetMemoryBarrierTransition( RHI::GlobalBarrier const& globalBarrier )
        {
            VkMemoryBarrierTransition barrier = {};
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

        VkBufferBarrierTransition VulkanCommandBuffer::GetBufferBarrierTransition( RHI::BufferBarrier const& bufferBarrier )
        {
            auto* pVkBuffer = RHI::RHIDowncast<VulkanBuffer>( bufferBarrier.m_pRhiBuffer );
            EE_ASSERT( pVkBuffer != nullptr );

            VkBufferBarrierTransition barrier = {};
            barrier.m_srcStage = VkFlags( 0 );
            barrier.m_dstStage = VkFlags( 0 );
            barrier.m_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.m_barrier.pNext = nullptr;
            barrier.m_barrier.srcAccessMask = VkFlags( 0 );
            barrier.m_barrier.dstAccessMask = VkFlags( 0 );
            barrier.m_barrier.srcQueueFamilyIndex = bufferBarrier.m_srcQueueFamilyIndex;
            barrier.m_barrier.dstQueueFamilyIndex = bufferBarrier.m_dstQueueFamilyIndex;
            barrier.m_barrier.buffer = pVkBuffer->m_pHandle;
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

        VkTextureBarrierTransition VulkanCommandBuffer::GetTextureBarrierTransition( RHI::TextureBarrier const& textureBarrier )
        {
            auto* pVkTexture = RHI::RHIDowncast<VulkanTexture>( textureBarrier.m_pRhiTexture );
            EE_ASSERT( pVkTexture != nullptr );

            VkTextureBarrierTransition barrier = {};
            barrier.m_srcStage = VkFlags( 0 );
            barrier.m_dstStage = VkFlags( 0 );
            barrier.m_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.m_barrier.pNext = nullptr;
            barrier.m_barrier.srcAccessMask = VkFlags( 0 );
            barrier.m_barrier.dstAccessMask = VkFlags( 0 );
            barrier.m_barrier.srcQueueFamilyIndex = textureBarrier.m_srcQueueFamilyIndex;
            barrier.m_barrier.dstQueueFamilyIndex = textureBarrier.m_dstQueueFamilyIndex;
            barrier.m_barrier.image = pVkTexture->m_pHandle;

            barrier.m_barrier.subresourceRange.aspectMask = ToVkImageAspectFlags( textureBarrier.m_subresourceRange.m_aspectFlags );
            barrier.m_barrier.subresourceRange.baseMipLevel = textureBarrier.m_subresourceRange.m_baseMipLevel;
            barrier.m_barrier.subresourceRange.levelCount = textureBarrier.m_subresourceRange.m_levelCount;
            barrier.m_barrier.subresourceRange.baseArrayLayer = textureBarrier.m_subresourceRange.m_baseArrayLayer;
            barrier.m_barrier.subresourceRange.layerCount = textureBarrier.m_subresourceRange.m_layerCount;

            for ( uint32_t i = 0; i < textureBarrier.m_previousAccessesCount; ++i )
            {
                auto const& prevAccess = textureBarrier.m_pPreviousAccesses[i];
                auto accessInfo = GetAccessInfo( prevAccess );

                // what stage this resource is used in previous stage
                barrier.m_srcStage |= accessInfo.m_stageMask;

                // only access the write access
                if ( IsWriteAccess( prevAccess ) )
                {
                    barrier.m_barrier.srcAccessMask |= accessInfo.m_accessMask;
                }

                if ( textureBarrier.m_discardContents )
                {
                    // we don't care about the previous image layout
                    barrier.m_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                }
                else
                {
                    switch ( textureBarrier.m_previousLayout )
                    {
                        case ImageMemoryLayout::Optimal:
                        barrier.m_barrier.newLayout = accessInfo.m_imageLayout;
                        break;
                        case ImageMemoryLayout::General:
                        {
                            if ( prevAccess == RHI::RenderResourceBarrierState::Present )
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

            for ( uint32_t i = 0; i < textureBarrier.m_nextAccessesCount; ++i )
            {
                auto const& nextAccess = textureBarrier.m_pNextAccesses[i];
                auto accessInfo = GetAccessInfo( nextAccess );

                // what stage this resource is used in previous stage
                barrier.m_dstStage |= accessInfo.m_stageMask;

                // if write access happend before, it must be visible to the dst access.
                // (i.e. RAW (Read-After-Write) operation or WAW)
                if ( barrier.m_barrier.srcAccessMask != VkFlags( 0 ) )
                {
                    barrier.m_barrier.dstAccessMask |= accessInfo.m_accessMask;
                }

                switch ( textureBarrier.m_nextLayout )
                {
                    case ImageMemoryLayout::Optimal:
                    barrier.m_barrier.oldLayout = accessInfo.m_imageLayout;
                    break;
                    case ImageMemoryLayout::General:
                    {
                        if ( nextAccess == RHI::RenderResourceBarrierState::Present )
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

#endif