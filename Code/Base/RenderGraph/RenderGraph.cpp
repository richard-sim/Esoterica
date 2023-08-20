#include "RenderGraph.h"
#include "Base/Logging/Log.h"
#include "Base/Threading/Threading.h"

namespace EE
{
	namespace RG
	{
		RGNodeBuilder::RGNodeBuilder( RenderGraph const& graph, RGNode& node )
			: m_graph( graph ), m_node( node )
		{}

		void RGNodeBuilder::RegisterRasterPipeline( Render::RasterPipelineDesc pipelineDesc )
		{
			EE_ASSERT( Threading::IsMainThread() );
			//EE_ASSERT( pipelineDesc.IsValid() );

            // Immediately register pipeline into graph, next frame it can start the loading process
            m_node.m_pipelineHandle = m_graph.RegisterRasterPipeline( pipelineDesc );
		}

		void RGNodeBuilder::RegisterComputePipeline( Render::ComputePipelineDesc pipelineDesc )
		{
			EE_ASSERT( Threading::IsMainThread() );
			//EE_ASSERT( pipelineDesc.IsValid() );
		}

		//-------------------------------------------------------------------------

		RenderGraph::RenderGraph()
			: RenderGraph( "No Name" )
		{}

		RenderGraph::RenderGraph( String const& graphName )
			: m_name( graphName )
        {}

        // Build Stage
		//-------------------------------------------------------------------------
		
		RGNodeBuilder RenderGraph::AddNode( String const& nodeName )
		{
			EE_ASSERT( Threading::IsMainThread() );

            auto nextID = static_cast<uint32_t>( m_graph.size() );
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

        // Compilation Stage
        //-------------------------------------------------------------------------

        void RenderGraph::Compile()
        {
            // Create actual RHI Resources
            //-------------------------------------------------------------------------

            for ( auto const& node : m_graph )
            {
                for ( auto const& input : node.m_pInputs )
                {
                    RGResource const& resource = GetRGResource( input );
                    
                    switch ( resource.GetResourceType() )
                    {
                        case RGResourceType::Buffer:
                        {
                            auto const& desc = resource.GetDesc<RGResourceTagBuffer>();
                            EE_LOG_MESSAGE( "RenderGraph", "RenderGraphCompilation", "Buffer Desired Size = %d", desc.m_desc.m_desireSize );
                            EE_LOG_MESSAGE( "RenderGraph", "RenderGraphCompilation", "Buffer Allocated Size = %d", desc.m_desc.m_desireSize );
                        }
                        break;

                        case RGResourceType::Texture:
                        {
                            auto const& desc = resource.GetDesc<RGResourceTagTexture>();
                            EE_LOG_MESSAGE( "RenderGraph", "RenderGraphCompilation", "Texture Width = %d", desc.m_desc.m_width );
                        }
                        break;

                        case RGResourceType::Unknown:
                        default:
                        EE_LOG_ERROR( "Render Graph", "Compilation", "Unknown type of render graph resource has been used in pass %s", node.m_passName );
                        EE_ASSERT( false );
                        break;
                    }
                }
            }

            // Compile and Analyze graph nodes to populate an execution sequence
            //-------------------------------------------------------------------------

        }

        // Execution Stage
        //-------------------------------------------------------------------------

        void RenderGraph::Execute()
        {

        }

        #endif

        //-------------------------------------------------------------------------
    }
}