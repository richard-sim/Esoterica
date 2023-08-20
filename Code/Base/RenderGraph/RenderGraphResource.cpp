#include "RenderGraphResource.h"

namespace EE::RG
{
    namespace _Impl
    {
		RGResourceSlotID::RGResourceSlotID( uint32_t id )
			: m_id( id )
		{}
    }

	//-------------------------------------------------------------------------

	RGResource::RGResource( _Impl::RGBufferDesc const& bufferDesc )
		: m_desc( bufferDesc )
	{}

	RGResource::RGResource( _Impl::RGTextureDesc const& textureDesc )
		: m_desc( textureDesc )
	{}
}