#pragma once

#include "Base/_Module/API.h"

#include "Base/Render/RenderAPI.h"
#include "Base/Types/BitFlags.h"
#include "Base/Types/RefCount.h"

//-------------------------------------------------------------------------
//	Rather than use sophisticated enum and bit flags inside vulkan or DX12
//	to perform resource barrier transform in order to synchronize. Render
//	graph resource barrier simplify this process and erase some of the invalid 
//	and nonsensical combinations of resource barrier.
// 
//	This idea comes from vk-sync-rs (by Graham Wihlidal) and be slightly modified.
//-------------------------------------------------------------------------

namespace EE
{
	namespace RG
	{
		enum class RGResourceBarrierState
		{
			/// Undefined resource state, primarily use for initialization.
			Undefined = 0,
			/// Read as an indirect buffer for drawing or dispatch
			IndirectBuffer,
			/// Read as a vertex buffer for drawing
			VertexBuffer,
			/// Read as an index buffer for drawing
			IndexBuffer,

			/// Read as a uniform buffer in a vertex shader
			VertexShaderReadUniformBuffer,
			/// Read as a sampled image/uniform texel buffer in a vertex shader
			VertexShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as any other resource in a vertex shader
			VertexShaderReadOther,

			/// Read as a uniform buffer in a tessellation control shader
			TessellationControlShaderReadUniformBuffer,
			/// Read as a sampled image/uniform texel buffer in a tessellation control shader
			TessellationControlShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as any other resource in a tessellation control shader
			TessellationControlShaderReadOther,

			/// Read as a uniform buffer in a tessellation evaluation shader
			TessellationEvaluationShaderReadUniformBuffer,
			/// Read as a sampled image/uniform texel buffer in a tessellation evaluation shader
			TessellationEvaluationShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as any other resource in a tessellation evaluation shader
			TessellationEvaluationShaderReadOther,

			/// Read as a uniform buffer in a geometry shader
			GeometryShaderReadUniformBuffer,
			/// Read as a sampled image/uniform texel buffer in a geometry shader
			GeometryShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as any other resource in a geometry shader
			GeometryShaderReadOther,

			/// Read as a uniform buffer in a fragment shader
			FragmentShaderReadUniformBuffer,
			/// Read as a sampled image/uniform texel buffer in a fragment shader
			FragmentShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as an input attachment with a color format in a fragment shader
			FragmentShaderReadColorInputAttachment,
			/// Read as an input attachment with a depth/stencil format in a fragment shader
			FragmentShaderReadDepthStencilInputAttachment,
			/// Read as any other resource in a fragment shader
			FragmentShaderReadOther,

			/// Read by blending/logic operations or subpass load operations
			ColorAttachmentRead,
			/// Read by depth/stencil tests or subpass load operations
			DepthStencilAttachmentRead,

			/// Read as a uniform buffer in a compute shader
			ComputeShaderReadUniformBuffer,
			/// Read as a sampled image/uniform texel buffer in a compute shader
			ComputeShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as any other resource in a compute shader
			ComputeShaderReadOther,

			/// Read as a uniform buffer in any shader
			AnyShaderReadUniformBuffer,
			/// Read as a uniform buffer in any shader, or a vertex buffer
			AnyShaderReadUniformBufferOrVertexBuffer,
			/// Read as a sampled image in any shader
			AnyShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as any other resource (excluding attachments) in any shader
			AnyShaderReadOther,

			/// Read as the source of a transfer operation
			TransferRead,
			/// Read on the host
			HostRead,
			/// Read by the presentation engine (i.e. `vkQueuePresentKHR`)
			Present,

			/// Written as any resource in a vertex shader
			VertexShaderWrite,
			/// Written as any resource in a tessellation control shader
			TessellationControlShaderWrite,
			/// Written as any resource in a tessellation evaluation shader
			TessellationEvaluationShaderWrite,
			/// Written as any resource in a geometry shader
			GeometryShaderWrite,
			/// Written as any resource in a fragment shader
			FragmentShaderWrite,

			/// Written as a color attachment during rendering, or via a subpass store op
			ColorAttachmentWrite,
			/// Written as a depth/stencil attachment during rendering, or via a subpass store op
			DepthStencilAttachmentWrite,
			/// Written as a depth aspect of a depth/stencil attachment during rendering, whilst the
			/// stencil aspect is read-only. Requires `VK_KHR_maintenance2` to be enabled.
			DepthAttachmentWriteStencilReadOnly,
			/// Written as a stencil aspect of a depth/stencil attachment during rendering, whilst the
			/// depth aspect is read-only. Requires `VK_KHR_maintenance2` to be enabled.
			StencilAttachmentWriteDepthReadOnly,

			/// Written as any resource in a compute shader
			ComputeShaderWrite,

			/// Written as any resource in any shader
			AnyShaderWrite,
			/// Written as the destination of a transfer operation
			TransferWrite,
			/// Written on the host
			HostWrite,

			/// Read or written as a color attachment during rendering
			ColorAttachmentReadWrite,
			/// Covers any access - useful for debug, generally avoid for performance reasons
			General,

			/// Read as a sampled image/uniform texel buffer in a ray tracing shader
			RayTracingShaderReadSampledImageOrUniformTexelBuffer,
			/// Read as an input attachment with a color format in a ray tracing shader
			RayTracingShaderReadColorInputAttachment,
			/// Read as an input attachment with a depth/stencil format in a ray tracing shader
			RayTracingShaderReadDepthStencilInputAttachment,
			/// Read as an acceleration structure in a ray tracing shader
			RayTracingShaderReadAccelerationStructure,
			/// Read as any other resource in a ray tracing shader
			RayTracingShaderReadOther,

			/// Written as an acceleration structure during acceleration structure building
			AccelerationStructureBuildWrite,
			/// Read as an acceleration structure during acceleration structure building (e.g. a BLAS when building a TLAS)
			AccelerationStructureBuildRead,
			/// Written as a buffer during acceleration structure building (e.g. a staging buffer)
			AccelerationStructureBufferWrite,
		};

		bool EE_BASE_API IsCommonReadOnlyAccess( RGResourceBarrierState const& access );
		bool EE_BASE_API IsCommonWriteAccess( RGResourceBarrierState const& access );

		bool EE_BASE_API IsRasterReadOnlyAccess( RGResourceBarrierState const& access );
		bool EE_BASE_API IsRasterWriteAccess( RGResourceBarrierState const& access );

		class EE_BASE_API RGResourceAccessState
		{
		public:

			RGResourceAccessState() = default;
			RGResourceAccessState( RGResourceBarrierState currentAccess, bool skipSyncIfContinuous = false );

			inline bool SkipSyncIfContinuous() const { return m_skipSyncIfContinuous; }

			inline RGResourceBarrierState GetCurrentAccess() const { return m_currentAccess; }
			inline void TransiteTo( RGResourceBarrierState state ) { m_currentAccess = state; }

		private:

			bool							m_skipSyncIfContinuous = false;
			RGResourceBarrierState			m_currentAccess = RGResourceBarrierState::Undefined;
		};

		//-------------------------------------------------------------------------

		enum class TextureLayout
		{
			/// Choose the most optimal layout for each usage. Performs layout transitions as appropriate for the access.
			Optimal,
			/// Layout accessible by all Vulkan access types on a device - no layout transitions except for presentation
			General,
			/// Similar to `General`, but also allows presentation engines to access it - no layout transitions.
			/// Requires `VK_KHR_shared_presentable_image` to be enabled, and this can only be used for shared presentable
			/// images (i.e. single-buffered swap chains).
			GeneralAndPresentation,
		};

		struct GlobalBarrier
		{
			uint32_t							m_previousAccessesCount;
			uint32_t							m_nextAccessesCount;
			RGResourceBarrierState const*		m_pPreviousAccesses;
			RGResourceBarrierState const*		m_pNextAccesses;
		};

		struct BufferBarrier
		{
			uint32_t							m_previousAccessesCount;
			uint32_t							m_nextAccessesCount;
			RGResourceBarrierState const*		m_pPreviousAccesses;
			RGResourceBarrierState const*		m_pNextAccesses;
			uint32_t							m_srcQueueFamilyIndex;
			uint32_t							m_dstQueueFamilyIndex;
			Render::BufferHandle				m_bufHandle;
			uint32_t							m_offset;
			uint32_t							m_size;
		};

		enum class ImageAspectFlags
		{
			Color,
			Depth,
			Stencil,
			Metadata,
		};

		struct TextureSubresourceRange
		{
			TBitFlags<ImageAspectFlags>		    m_aspectFlags;
			uint32_t							m_baseMipLevel;
			uint32_t							m_levelCount;
			uint32_t							m_baseArrayLayer;
			uint32_t							m_layerCount;
		};

		struct ImageBarrier
		{
			bool								m_discardContents;
			uint32_t							m_previousAccessesCount;
			uint32_t							m_nextAccessesCount;
			RGResourceBarrierState const*		m_pPreviousAccesses;
			RGResourceBarrierState const*		m_pNextAccesses;
			TextureLayout						m_previousLayout;
			TextureLayout						m_nextLayout;
			uint32_t							m_srcQueueFamilyIndex;
			uint32_t							m_dstQueueFamilyIndex;
			Render::TextureHandle				m_texHandle;
			TextureSubresourceRange				m_subresourceRange;
		};

		//#ifdef EE_VULKAN
		//class VulkanDevice;

		//typedef RefCountPtr<VulkanDevice> DevicePtr;
		//#endif

		//void PipelineBarrier( 
		//	DevicePtr& pDevice, Render::CommandBufferHandle cmdBuf,
		//	uint32_t globalBarrierCount, GlobalBarrier const* pGlobalBarriers,
		//	uint32_t bufferBarrierCount, BufferBarrier const* pBufferBarriers,
		//	uint32_t textureBarrierCount, ImageBarrier const* pTextureBarriers
		//);
	}
}