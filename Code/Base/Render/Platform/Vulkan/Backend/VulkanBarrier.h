#pragma once
#if defined(EE_VULKAN)

#include "Base/Render/RenderResourceBarrier.h"
#include "VulkanCommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace EE::Render
{
	namespace Backend
	{
		struct MemoryBarrierTransition
		{
			VkPipelineStageFlags				m_srcStage;
			VkPipelineStageFlags				m_dstStage;
			VkMemoryBarrier						m_barrier;
		};

		struct BufferBarrierTransition
		{
			VkPipelineStageFlags				m_srcStage;
			VkPipelineStageFlags				m_dstStage;
			VkBufferMemoryBarrier				m_barrier;
		};

		struct ImageBarrierTransition
		{
			VkPipelineStageFlags				m_srcStage;
			VkPipelineStageFlags				m_dstStage;
			VkImageMemoryBarrier				m_barrier;
		};

		MemoryBarrierTransition GetMemoryBarrierTransition( GlobalBarrier const& globalBarrier );
		BufferBarrierTransition GetBufferBarrierTransition( BufferBarrier const& bufferBarrier );
		ImageBarrierTransition GetImageBarrierTransition( ImageBarrier const& imageBarrier );

		//-------------------------------------------------------------------------

		void VulkanCmdPipelineBarrier(
			VulkanCommandBuffer* pCmdBuf,
			GlobalBarrier const* pGlobalBarriers,
			uint32_t bufferBarrierCount, BufferBarrier const* pBufferBarriers,
			uint32_t imageBarrierCount, ImageBarrier const* pImageBarriers
		);
	}
}

#endif // EE_VULKAN