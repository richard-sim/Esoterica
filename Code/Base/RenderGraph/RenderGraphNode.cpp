#include "RenderGraphNode.h"

namespace EE
{
	namespace RG
	{
		RGNode::RGNode()
			: RGNode( "Unknown Pass", eastl::numeric_limits<NodeID>::max() )
		{}

		RGNode::RGNode( String const& nodeName, NodeID id )
			: m_passName( nodeName ), m_id( id )
		{}

		//-------------------------------------------------------------------------

		RGNodeResource::RGNodeResource( _Impl::RGResourceID slotID, Render::RenderResourceAccessState access )
			: m_slotID( slotID ), m_passAccess( access )
		{}

		//-------------------------------------------------------------------------
	}
}