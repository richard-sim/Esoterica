#include "RenderGraph.h"
#include "System/Log.h"

namespace EE
{
	namespace RG
	{
		RenderGraph::RenderGraph()
			: RenderGraph( "Unknown Render Graph" )
		{}

		RenderGraph::RenderGraph( String const& graphName )
			: m_name( graphName )
		{}

		//-------------------------------------------------------------------------
		
		RGNode& RenderGraph::AddNode( String const& nodeName )
		{
			return m_graph.emplace_back( nodeName );
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