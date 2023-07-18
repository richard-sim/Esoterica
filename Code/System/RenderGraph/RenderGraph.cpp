#include "RenderGraph.h"
#include "System/Logging/Log.h"
#include "System/Threading/Threading.h"

namespace EE
{
	namespace RG
	{
		RGNodeBuilder::RGNodeBuilder( RenderGraph const& graph, RGNode& node )
			: m_graph( graph ), m_node( node )
		{}

		void RGNodeBuilder::RegisterRasterPipeline( RasterPipelineDesc pipelineDesc )
		{
			EE_ASSERT( Threading::IsMainThread() );
			EE_ASSERT( pipelineDesc.IsValid() );
		}

		void RGNodeBuilder::RegisterComputePipeline( ComputePipelineDesc pipelineDesc )
		{
			EE_ASSERT( Threading::IsMainThread() );
			//EE_ASSERT( pipelineDesc.IsValid() );
		}

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
			EE_ASSERT( Threading::IsMainThread() );

			auto nextID = static_cast<uint32_t>(m_graph.size());
			auto& newNode = m_graph.emplace_back( nodeName, nextID );

			return RGNodeBuilder( *this, newNode );
		}

		#if EE_DEVELOPMENT_TOOLS
		void RenderGraph::LogGraphNodes() const
		{
			EE_ASSERT( Threading::IsMainThread() );

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