#include "RenderGraphNode.h"

namespace EE
{
	namespace RG
	{
		RGNode::RGNode()
			: RGNode( "Unknown Pass" )
		{}

		RGNode::RGNode( String const& nodeName )
			: m_passName( nodeName )
		{}

		//-------------------------------------------------------------------------
	
		RGNodeResource::RGNodeResource( RGResourceSlotID slotID )
			: m_slotID( slotID )
		{
		}
	}
}