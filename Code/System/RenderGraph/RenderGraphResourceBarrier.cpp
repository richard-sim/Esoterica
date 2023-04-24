#include "RenderGraphResourceBarrier.h"

namespace EE
{
	namespace RG
	{
		bool IsCommonReadOnlyAccess( RGResourceBarrierState const& access )
		{
			switch ( access )
			{
				case RGResourceBarrierState::IndirectBuffer:
				case RGResourceBarrierState::VertexBuffer:
				case RGResourceBarrierState::IndexBuffer:
				case RGResourceBarrierState::VertexShaderReadUniformBuffer:
				case RGResourceBarrierState::VertexShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::VertexShaderReadOther:
				case RGResourceBarrierState::TessellationControlShaderReadUniformBuffer:
				case RGResourceBarrierState::TessellationControlShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::TessellationControlShaderReadOther:
				case RGResourceBarrierState::TessellationEvaluationShaderReadUniformBuffer:
				case RGResourceBarrierState::TessellationEvaluationShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::TessellationEvaluationShaderReadOther:
				case RGResourceBarrierState::GeometryShaderReadUniformBuffer:
				case RGResourceBarrierState::GeometryShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::GeometryShaderReadOther:
				case RGResourceBarrierState::FragmentShaderReadUniformBuffer:
				case RGResourceBarrierState::FragmentShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::FragmentShaderReadColorInputAttachment:
				case RGResourceBarrierState::FragmentShaderReadDepthStencilInputAttachment:
				case RGResourceBarrierState::FragmentShaderReadOther:
				case RGResourceBarrierState::ColorAttachmentRead:
				case RGResourceBarrierState::DepthStencilAttachmentRead:
				case RGResourceBarrierState::ComputeShaderReadUniformBuffer:
				case RGResourceBarrierState::ComputeShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::ComputeShaderReadOther:
				case RGResourceBarrierState::AnyShaderReadUniformBuffer:
				case RGResourceBarrierState::AnyShaderReadUniformBufferOrVertexBuffer:
				case RGResourceBarrierState::AnyShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::AnyShaderReadOther:
				case RGResourceBarrierState::TransferRead:
				case RGResourceBarrierState::HostRead:
				case RGResourceBarrierState::Present:

				case RGResourceBarrierState::RayTracingShaderReadSampledImageOrUniformTexelBuffer:
				case RGResourceBarrierState::RayTracingShaderReadColorInputAttachment:
				case RGResourceBarrierState::RayTracingShaderReadDepthStencilInputAttachment:
				case RGResourceBarrierState::RayTracingShaderReadAccelerationStructure:
				case RGResourceBarrierState::RayTracingShaderReadOther:

				case RGResourceBarrierState::AccelerationStructureBuildRead:

				return true;
				break;

				default:
				return false;
				break;
			}
		}

		bool IsCommonWriteAccess( RGResourceBarrierState const& access )
		{
			switch ( access )
			{
				case RGResourceBarrierState::VertexShaderWrite:
				case RGResourceBarrierState::TessellationControlShaderWrite:
				case RGResourceBarrierState::TessellationEvaluationShaderWrite:
				case RGResourceBarrierState::GeometryShaderWrite:
				case RGResourceBarrierState::FragmentShaderWrite:
				case RGResourceBarrierState::ColorAttachmentWrite:
				case RGResourceBarrierState::DepthStencilAttachmentWrite:
				case RGResourceBarrierState::DepthAttachmentWriteStencilReadOnly:
				case RGResourceBarrierState::StencilAttachmentWriteDepthReadOnly:
				case RGResourceBarrierState::ComputeShaderWrite:
				case RGResourceBarrierState::AnyShaderWrite:
				case RGResourceBarrierState::TransferWrite:
				case RGResourceBarrierState::HostWrite:

				// TODO: Should we put General in write access?
				case RGResourceBarrierState::General:

				case RGResourceBarrierState::ColorAttachmentReadWrite:

				case RGResourceBarrierState::AccelerationStructureBuildWrite:
				case RGResourceBarrierState::AccelerationStructureBufferWrite:

				return true;
				break;

				default:
				return false;
				break;
			}
		}

		bool IsRasterReadOnlyAccess( RGResourceBarrierState const& access )
		{
			switch ( access )
			{
				case RGResourceBarrierState::ColorAttachmentRead:
				case RGResourceBarrierState::DepthStencilAttachmentRead:

				return true;
				break;

				default:
				return false;
				break;
			}
		}

		bool IsRasterWriteAccess( RGResourceBarrierState const& access )
		{
			switch ( access )
			{
				case RGResourceBarrierState::ColorAttachmentWrite:
				case RGResourceBarrierState::DepthStencilAttachmentWrite:
				case RGResourceBarrierState::DepthAttachmentWriteStencilReadOnly:
				case RGResourceBarrierState::StencilAttachmentWriteDepthReadOnly:

				case RGResourceBarrierState::ColorAttachmentReadWrite:

				return true;
				break;

				default:
				return false;
				break;
			}
		}

		//-------------------------------------------------------------------------
	
		RGResourceAccessState::RGResourceAccessState( RGResourceBarrierState currentAccess, bool skipSyncIfContinuous )
			: m_skipSyncIfContinuous( skipSyncIfContinuous ), m_currentAccess( currentAccess )
		{}
	}
}