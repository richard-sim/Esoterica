#include "RenderGraph.h"
#include "Base/Logging/Log.h"
#include "Base/Threading/Threading.h"
#include "Base/RHI/RHIDevice.h"
#include "Base/RHI/Resource/RHIBuffer.h"
#include "Base/RHI/Resource/RHITexture.h"

namespace EE
{
	namespace RG
	{
		RGNodeBuilder::RGNodeBuilder( RenderGraph const& graph, RGNode& node )
			: m_graph( graph ), m_node( node )
		{}

		void RGNodeBuilder::RegisterRasterPipeline( RHI::RHIRasterPipelineStateCreateDesc pipelineDesc )
		{
			EE_ASSERT( Threading::IsMainThread() );
			EE_ASSERT( pipelineDesc.IsValid() );

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

        void RenderGraph::Compile( RHI::RHIDevice* pRhiDevice )
        {
            if ( pRhiDevice == nullptr )
            {
                EE_LOG_WARNING("RenderGraph", "RenderGraph::Compile()", "RHI Device missing! Cannot compile render graph!");
                return;
            }

            // Create actual RHI Resources
            //-------------------------------------------------------------------------

            CreateRHIResource( pRhiDevice );

            // Compile and Analyze graph nodes to populate an execution sequence
            //-------------------------------------------------------------------------
        }

        void RenderGraph::CreateRHIResource( RHI::RHIDevice* pRhiDevice )
        {
            EE_ASSERT( pRhiDevice != nullptr );

            auto CreateRHILazyResource = [pRhiDevice] ( RGResource& rgResource )
            {
                switch ( rgResource.GetResourceType() )
                {
                    case RGResourceType::Buffer:
                    {
                        BufferDesc const& desc = rgResource.GetDesc<RGResourceTagBuffer>();

                        EE_LOG_MESSAGE( "RenderGraph", "RenderGraph::CreateNodeRHIResource()", "Buffer Desired Size = %d", desc.m_desc.m_desireSize );
                        EE_LOG_MESSAGE( "RenderGraph", "RenderGraph::CreateNodeRHIResource()", "Buffer Allocated Size = %d", desc.m_desc.m_desireSize );

                        if ( rgResource.IsLazyCreateResource() )
                        {
                            RGLazyCreateResource& rhiResource = rgResource.GetLazyCreateResource();
                            rhiResource.m_pRhiResource = pRhiDevice->CreateBuffer( desc.m_desc );
                            EE_ASSERT( rhiResource.m_pRhiResource != nullptr );
                        }
                    }
                    break;

                    case RGResourceType::Texture:
                    {
                        TextureDesc const& desc = rgResource.GetDesc<RGResourceTagTexture>();

                        EE_LOG_MESSAGE( "RenderGraph", "RenderGraph::CreateNodeRHIResource()", "Texture Width = %d", desc.m_desc.m_width );
                        EE_LOG_MESSAGE( "RenderGraph", "RenderGraph::CreateNodeRHIResource()", "Texture Height = %d", desc.m_desc.m_height );

                        if ( rgResource.IsLazyCreateResource() )
                        {
                            RGLazyCreateResource& rhiResource = rgResource.GetLazyCreateResource();
                            rhiResource.m_pRhiResource = pRhiDevice->CreateTexture( desc.m_desc );
                            EE_ASSERT( rhiResource.m_pRhiResource != nullptr );
                        }
                    }
                    break;

                    case RGResourceType::Unknown:
                    default:
                    EE_LOG_ERROR( "Render Graph", "RenderGraph::Compile()", "Unknown type of render graph resource!" );
                    EE_ASSERT( false );
                    break;
                }
            };

            for ( auto& rgResource : m_graphResources )
            {
                CreateRHILazyResource( rgResource );
            }

            // Compile nodes into executable nodes
            // TODO: graph dependency analyze
            //-------------------------------------------------------------------------

            for ( RGNode const& node : m_graph )
            {
                
            }
        }

        // Execution Stage
        //-------------------------------------------------------------------------

        void RenderGraph::Execute()
        {

        }

        // Cleanup Stage
        //-------------------------------------------------------------------------

        void RenderGraph::ClearAllRHIResources( RHI::RHIDevice* pRhiDevice )
        {
            EE_ASSERT( pRhiDevice != nullptr );

            for ( auto& resource : m_graphResources )
            {
                switch ( resource.GetResourceType() )
                {
                    case RGResourceType::Buffer:
                    {
                        if ( resource.IsLazyCreateResource() )
                        {
                            auto& rhiResource = resource.GetLazyCreateResource();
                            RHI::RHIBuffer* pRhiBuffer = static_cast<RHI::RHIBuffer*>( rhiResource.m_pRhiResource );
                            pRhiDevice->DestroyBuffer( pRhiBuffer );
                            rhiResource.m_pRhiResource = nullptr;
                        }
                    }
                    break;

                    case RGResourceType::Texture:
                    {
                        auto& rhiResource = resource.GetLazyCreateResource();
                        RHI::RHITexture* pRhiTexture = static_cast<RHI::RHITexture*>( rhiResource.m_pRhiResource );
                        pRhiDevice->DestroyTexture( pRhiTexture );
                        rhiResource.m_pRhiResource = nullptr;
                    }
                    break;

                    case RGResourceType::Unknown:
                    default:
                    EE_LOG_ERROR( "Render Graph", "Compilation", "Unknown type of render graph resource!" );
                    EE_ASSERT( false );
                    break;
                }
            }

            m_graphResources.clear();
        }

        #endif

        //-------------------------------------------------------------------------
    }
}