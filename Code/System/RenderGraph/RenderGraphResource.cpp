#include "RenderGraphResource.h"

namespace EE
{
	namespace RG
	{
		RGResourceSlotID::RGResourceSlotID( uint32_t id )
			: m_id( id )
		{
		}

		//-------------------------------------------------------------------------

		RGResource::RGResource( BufferDescBaseType bufferDesc )
			: m_desc( bufferDesc )
		{}

		RGResource::RGResource( TextureDescBaseType textureDesc )
			: m_desc( textureDesc )
		{}
	}
}