#include "RenderGraphResource.h"

namespace EE
{
	namespace RG
	{
		RGResourceSlotID::RGResourceSlotID( uint32_t id )
			: m_id( id )
		{}

		//-------------------------------------------------------------------------

		RGResource::RGResource( RGBufferDesc const& bufferDesc )
			: m_desc( bufferDesc )
		{}

		RGResource::RGResource( RGTextureDesc const& textureDesc )
			: m_desc( textureDesc )
		{}
	}
}