#include "RenderGraph.h"
#include "System/Log.h"

namespace EE
{
	namespace RG
	{
		RGNodeBuilder::RGNodeBuilder( RenderGraph& graph, RGNode& node )
			: m_graph( graph ), m_node( node )
		{}

		//-------------------------------------------------------------------------

		RenderGraph::RenderGraph()
			: RenderGraph( "Unknown Render Graph" )
		{}

		RenderGraph::RenderGraph( String const& graphName )
			: m_name( graphName )
		{}

		//-------------------------------------------------------------------------
		
		RGNodeBuilder RenderGraph::AddNode( String const& nodeName )
		{
			auto nextID = static_cast<uint32_t>(m_graph.size());
			auto& newNode = m_graph.emplace_back( nodeName, nextID );

			return RGNodeBuilder( *this, newNode );
		}

		#if EE_DEVELOPMENT_TOOLS
		void RenderGraph::LogGraphNodes() const
		{
			size_t const count = m_graph.size();

			EE_LOG_MESSAGE( "Render Graph", "Graph", "Node Count: %u", count );

			for ( size_t i = 0; i < count; ++i )
			{
				EE_LOG_MESSAGE( "Render Graph", "Graph", "\tNode (%s)", m_graph[i].m_passName.c_str() );
			}
		}
		#endif
	}
}