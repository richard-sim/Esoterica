#pragma once

//-------------------------------------------------------------------------
//	Rather than use sophisticated enum and bit flags inside vulkan or DX12
//	to perform resource barrier transform in order to synchronize. Render
//	graph resource barrier simplify this process and erase some of the invalid 
//	and nonsensical combinations of resource barrier.
// 
//	This idea comes from vk-sync-rs and modified by myself.
//-------------------------------------------------------------------------

namespace EE
{
	namespace RG
	{
		enum class RGResourceBarrierState
		{
			Undefined, // undefined resource state, primarily use for initialization.

			VertexBuffer, // read as a vertex buffer for drawing
			IndexBuffer, // read as an index buffer for drawing
		};
	}
}